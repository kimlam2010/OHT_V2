# 🔧 SENSOR FUSION ALGORITHM - CHI TIẾT KỸ THUẬT

**Phiên bản:** 1.0  
**Ngày:** 2025-01-28  
**Mục tiêu:** Chi tiết thuật toán sensor fusion cho Map & Localization  
**Sensors:** RFID + Accelerometer + Proximity + LiDAR  

---

## 📋 **TỔNG QUAN SENSOR FUSION**

### **🎯 Mục tiêu:**
- Kết hợp dữ liệu từ 4 nguồn sensors
- Tối ưu độ chính xác và độ tin cậy
- Xử lý trường hợp sensors bị lỗi
- Cung cấp localization robust

### **🔧 Sensors sử dụng:**
1. **RFID Reader:** Vị trí tuyệt đối từ RFID tags
2. **Accelerometer:** Gia tốc 3 trục (x, y, z)
3. **Proximity Sensors:** 2 cảm biến tiệm cận vào dock
4. **LiDAR HAL:** Quét môi trường 360°

---

## 🧮 **SENSOR FUSION ALGORITHM**

### **1. RFID Localization:**
```python
def estimate_from_rfid(rfid_data: RFIDData) -> RobotPose:
    """Estimate robot position from RFID data"""
    if rfid_data.rfid_id in rfid_tag_database:
        tag_info = rfid_tag_database[rfid_data.rfid_id]
        return RobotPose(
            x=tag_info.position_x,
            y=tag_info.position_y,
            theta=tag_info.orientation,
            confidence=0.95,  # High confidence for RFID
            source="rfid"
        )
    else:
        return RobotPose(confidence=0.0, source="rfid")
```

### **2. Sensor Fusion (Accelerometer + Proximity):**
```python
def estimate_from_sensor_fusion(sensor_data: SensorData, last_pose: RobotPose) -> RobotPose:
    """Estimate robot position from accelerometer and proximity sensors"""
    
    # Step 1: Check proximity sensors for dock detection
    if sensor_data.proximity_sensor_1 and sensor_data.proximity_sensor_2:
        # Both proximity sensors active - likely at dock
        dock_confidence = 0.8
        estimated_pose = last_known_dock_position
    else:
        dock_confidence = 0.0
        estimated_pose = None
    
    # Step 2: Use accelerometer for movement estimation
    if last_pose is not None:
        # Calculate movement from accelerometer
        dt = (sensor_data.timestamp - last_pose.timestamp).total_seconds()
        
        # Simple integration (can be improved with Kalman filter)
        velocity_x = last_pose.velocity_x + sensor_data.accelerometer_x * dt
        velocity_y = last_pose.velocity_y + sensor_data.accelerometer_y * dt
        
        # Update position
        new_x = last_pose.x + velocity_x * dt
        new_y = last_pose.y + velocity_y * dt
        
        # Estimate orientation from accelerometer
        new_theta = math.atan2(sensor_data.accelerometer_y, sensor_data.accelerometer_x)
        
        estimated_pose = RobotPose(
            x=new_x,
            y=new_y,
            theta=new_theta,
            confidence=0.6,  # Medium confidence for sensor fusion
            source="sensor_fusion"
        )
    
    # Step 3: Combine dock detection and movement estimation
    if dock_confidence > 0.5:
        # Use dock position if detected
        final_pose = last_known_dock_position
        final_pose.confidence = dock_confidence
        final_pose.source = "sensor_fusion_dock"
    else:
        # Use movement estimation
        final_pose = estimated_pose
        final_pose.confidence = 0.4  # Lower confidence without dock
        final_pose.source = "sensor_fusion_movement"
    
    return final_pose
```

### **3. LiDAR Localization:**
```python
def estimate_from_lidar(lidar_data: LiDARScan, current_map: MapData) -> RobotPose:
    """Estimate robot position from LiDAR scan"""
    
    if not current_map or not lidar_data.points:
        return RobotPose(confidence=0.0, source="lidar")
    
    # Step 1: Convert LiDAR points to map coordinates
    map_points = []
    for point in lidar_data.points:
        if point.quality > 0.5:  # Only use high quality points
            # Convert polar to Cartesian (relative to robot)
            x_rel = point.distance * math.cos(math.radians(point.angle))
            y_rel = point.distance * math.sin(math.radians(point.angle))
            map_points.append((x_rel, y_rel))
    
    # Step 2: Match with existing map (simplified ICP algorithm)
    best_match = None
    best_score = 0.0
    
    # Try different positions around last known position
    for dx in range(-500, 501, 50):  # 50mm steps
        for dy in range(-500, 501, 50):
            for dtheta in range(-180, 181, 10):  # 10 degree steps
                test_pose = RobotPose(
                    x=last_known_pose.x + dx,
                    y=last_known_pose.y + dy,
                    theta=last_known_pose.theta + math.radians(dtheta)
                )
                
                # Calculate match score
                score = calculate_match_score(map_points, test_pose, current_map)
                if score > best_score:
                    best_score = score
                    best_match = test_pose
    
    if best_match and best_score > 0.3:
        best_match.confidence = min(best_score, 0.7)  # Cap at 0.7
        best_match.source = "lidar"
        return best_match
    else:
        return RobotPose(confidence=0.0, source="lidar")
```

### **4. Hybrid Fusion Algorithm:**
```python
def fuse_estimates(rfid_pose: RobotPose, sensor_pose: RobotPose, lidar_pose: RobotPose) -> RobotPose:
    """Fuse all estimates using weighted average"""
    
    # Calculate weights based on confidence
    total_weight = 0.0
    weighted_x = 0.0
    weighted_y = 0.0
    weighted_theta = 0.0
    
    # RFID weight
    if rfid_pose.confidence > 0.5:
        weight = rfid_pose.confidence * 0.5  # RFID gets 50% weight
        total_weight += weight
        weighted_x += rfid_pose.x * weight
        weighted_y += rfid_pose.y * weight
        weighted_theta += rfid_pose.theta * weight
    
    # Sensor fusion weight
    if sensor_pose.confidence > 0.3:
        weight = sensor_pose.confidence * 0.3  # Sensor fusion gets 30% weight
        total_weight += weight
        weighted_x += sensor_pose.x * weight
        weighted_y += sensor_pose.y * weight
        weighted_theta += sensor_pose.theta * weight
    
    # LiDAR weight
    if lidar_pose.confidence > 0.2:
        weight = lidar_pose.confidence * 0.2  # LiDAR gets 20% weight
        total_weight += weight
        weighted_x += lidar_pose.x * weight
        weighted_y += lidar_pose.y * weight
        weighted_theta += lidar_pose.theta * weight
    
    if total_weight > 0:
        # Calculate weighted average
        final_pose = RobotPose(
            x=weighted_x / total_weight,
            y=weighted_y / total_weight,
            theta=weighted_theta / total_weight,
            confidence=min(total_weight, 0.9),  # Cap at 0.9
            source="hybrid"
        )
        return final_pose
    else:
        # No reliable estimates
        return RobotPose(confidence=0.0, source="hybrid")
```

---

## 📊 **CONFIDENCE CALCULATION**

### **1. RFID Confidence:**
```python
def calculate_rfid_confidence(rfid_data: RFIDData) -> float:
    """Calculate confidence for RFID localization"""
    base_confidence = 0.95
    
    # Reduce confidence if signal is weak
    if rfid_data.signal_strength < 0.5:
        base_confidence *= 0.8
    
    # Reduce confidence if multiple tags detected
    if rfid_data.multiple_tags:
        base_confidence *= 0.7
    
    return min(base_confidence, 0.95)
```

### **2. Sensor Fusion Confidence:**
```python
def calculate_sensor_fusion_confidence(sensor_data: SensorData, last_pose: RobotPose) -> float:
    """Calculate confidence for sensor fusion"""
    base_confidence = 0.6
    
    # Increase confidence if at dock
    if sensor_data.proximity_sensor_1 and sensor_data.proximity_sensor_2:
        base_confidence = 0.8
    
    # Reduce confidence if accelerometer data is noisy
    acceleration_magnitude = math.sqrt(
        sensor_data.accelerometer_x**2 + 
        sensor_data.accelerometer_y**2 + 
        sensor_data.accelerometer_z**2
    )
    
    if acceleration_magnitude > 10.0:  # High acceleration
        base_confidence *= 0.7
    
    # Reduce confidence over time (drift)
    if last_pose:
        time_since_last = (sensor_data.timestamp - last_pose.timestamp).total_seconds()
        if time_since_last > 5.0:  # 5 seconds
            base_confidence *= 0.8
    
    return min(base_confidence, 0.8)
```

### **3. LiDAR Confidence:**
```python
def calculate_lidar_confidence(lidar_data: LiDARScan, match_score: float) -> float:
    """Calculate confidence for LiDAR localization"""
    base_confidence = match_score
    
    # Reduce confidence if few points
    if len(lidar_data.points) < 50:
        base_confidence *= 0.5
    
    # Reduce confidence if low quality points
    high_quality_points = sum(1 for p in lidar_data.points if p.quality > 0.5)
    if high_quality_points < len(lidar_data.points) * 0.7:
        base_confidence *= 0.7
    
    return min(base_confidence, 0.7)
```

---

## 🔄 **FALLBACK STRATEGIES**

### **1. Single Sensor Failure:**
```python
def handle_sensor_failure(failed_sensor: str, available_estimates: List[RobotPose]) -> RobotPose:
    """Handle failure of a single sensor"""
    
    if failed_sensor == "rfid":
        # Use sensor fusion + LiDAR
        return fuse_estimates(
            RobotPose(confidence=0.0),  # No RFID
            available_estimates[1],     # Sensor fusion
            available_estimates[2]      # LiDAR
        )
    
    elif failed_sensor == "sensor_fusion":
        # Use RFID + LiDAR
        return fuse_estimates(
            available_estimates[0],     # RFID
            RobotPose(confidence=0.0),  # No sensor fusion
            available_estimates[2]      # LiDAR
        )
    
    elif failed_sensor == "lidar":
        # Use RFID + sensor fusion
        return fuse_estimates(
            available_estimates[0],     # RFID
            available_estimates[1],     # Sensor fusion
            RobotPose(confidence=0.0)   # No LiDAR
        )
```

### **2. Multiple Sensor Failure:**
```python
def handle_multiple_sensor_failure(available_estimates: List[RobotPose]) -> RobotPose:
    """Handle failure of multiple sensors"""
    
    # Find the best available estimate
    best_estimate = max(available_estimates, key=lambda x: x.confidence)
    
    if best_estimate.confidence > 0.3:
        return best_estimate
    else:
        # All sensors failed - use last known position
        return RobotPose(
            x=last_known_pose.x,
            y=last_known_pose.y,
            theta=last_known_pose.theta,
            confidence=0.1,
            source="fallback"
        )
```

---

## 📈 **PERFORMANCE OPTIMIZATION**

### **1. Computational Efficiency:**
- **RFID:** O(1) - Direct lookup
- **Sensor Fusion:** O(1) - Simple integration
- **LiDAR:** O(n²) - Grid matching (can be optimized)
- **Fusion:** O(1) - Weighted average

### **2. Memory Usage:**
- **RFID Database:** ~1KB per tag
- **Sensor History:** ~100KB for 1000 samples
- **LiDAR Points:** ~10KB per scan
- **Total:** ~111KB per robot

### **3. Real-time Requirements:**
- **RFID Processing:** < 1ms
- **Sensor Fusion:** < 5ms
- **LiDAR Processing:** < 50ms
- **Fusion:** < 1ms
- **Total:** < 57ms (meets 100ms requirement)

---

## 🧪 **TESTING STRATEGY**

### **1. Unit Tests:**
- Individual sensor estimation functions
- Confidence calculation functions
- Fusion algorithm
- Fallback strategies

### **2. Integration Tests:**
- Complete sensor fusion pipeline
- Real sensor data simulation
- Performance benchmarks
- Error handling

### **3. Field Tests:**
- RFID tag detection accuracy
- Accelerometer drift measurement
- Proximity sensor reliability
- LiDAR scan quality
- Overall localization accuracy

---

## 📊 **EXPECTED ACCURACY**

### **1. Individual Sensors:**
- **RFID:** ±10mm (when tag detected)
- **Accelerometer:** ±100mm (after 10 seconds)
- **Proximity:** ±5mm (when at dock)
- **LiDAR:** ±50mm (with good map)

### **2. Sensor Fusion:**
- **RFID + Proximity:** ±8mm
- **RFID + Accelerometer:** ±15mm
- **All sensors:** ±12mm
- **Fallback:** ±200mm

### **3. Reliability:**
- **Single sensor failure:** 95% accuracy maintained
- **Multiple sensor failure:** 80% accuracy maintained
- **All sensors failed:** 10% accuracy (fallback)

---

**Changelog v1.0:**
- ✅ Created detailed sensor fusion algorithm
- ✅ Defined confidence calculation methods
- ✅ Implemented fallback strategies
- ✅ Specified performance requirements
- ✅ Outlined testing strategy
- ✅ Set accuracy expectations
