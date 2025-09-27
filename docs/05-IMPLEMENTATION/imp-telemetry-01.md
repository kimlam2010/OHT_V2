# IMPLEMENTATION GUIDE: TELEMETRY SYSTEM - PHASE 3

**Phiên bản:** 1.0  
**Ngày cập nhật:** 2025-01-28  
**Phase:** Phase 3 - Core Features & Business Logic  
**Status:** In Development  

---

## 🎯 **OVERVIEW**

### **Mục tiêu:**
Xây dựng real-time telemetry system với data processing, performance metrics, và alert system để monitor robot performance và system health.

### **Phạm vi:**
- Real-time data collection từ sensors và LiDAR
- Data processing với filtering và aggregation
- Performance metrics collection và analysis
- Alert system với threshold monitoring

---

## 🏗️ **ARCHITECTURE**

### **System Components:**
```
┌─────────────────────────────────────┐
│         Telemetry System            │
├─────────────────────────────────────┤
│  ┌─────────────┐  ┌─────────────┐  │
│  │    Data     │  │    Data     │  │
│  │ Collection  │  │ Processing  │  │
│  └─────────────┘  └─────────────┘  │
│  ┌─────────────┐  ┌─────────────┐  │
│  │ Performance │  │    Alert    │  │
│  │  Metrics    │  │   System    │  │
│  └─────────────┘  └─────────────┘  │
└─────────────────────────────────────┘
```

### **Data Flow:**
```
Sensors → Data Collection → Processing → Storage → Analysis → Alerts
   ↓           ↓             ↓          ↓         ↓         ↓
LiDAR    → Validation → Filtering → Database → Metrics → Notifications
```

---

## 🔧 **IMPLEMENTATION DETAILS**

### **1. Data Collection System**

#### **Sensor Data Integration:**
```python
class SensorDataCollector:
    def __init__(self, collection_frequency=100):  # 100Hz
        self.collection_frequency = collection_frequency
        self.sensors = {}
        self.collection_active = False
        
    async def register_sensor(self, sensor_id: str, sensor_type: str, 
                            data_format: dict):
        """Register a new sensor for data collection"""
        self.sensors[sensor_id] = {
            'type': sensor_type,
            'format': data_format,
            'last_reading': None,
            'status': 'active'
        }
        
    async def start_collection(self):
        """Start continuous data collection"""
        self.collection_active = True
        asyncio.create_task(self._collect_data_loop())
        
    async def _collect_data_loop(self):
        """Main data collection loop"""
        while self.collection_active:
            for sensor_id, sensor_info in self.sensors.items():
                if sensor_info['status'] == 'active':
                    try:
                        data = await self._read_sensor(sensor_id)
                        await self._process_sensor_data(sensor_id, data)
                    except Exception as e:
                        logger.error(f"Error reading sensor {sensor_id}: {e}")
                        sensor_info['status'] = 'error'
                        
            await asyncio.sleep(1.0 / self.collection_frequency)
```

#### **LiDAR Data Processing:**
```python
class LiDARDataProcessor:
    def __init__(self, max_points=3600, processing_frequency=10):
        self.max_points = max_points
        self.processing_frequency = processing_frequency
        self.point_cloud_buffer = []
        self.obstacle_detector = ObstacleDetector()
        
    async def process_lidar_data(self, raw_data: bytes) -> dict:
        """Process raw LiDAR data into structured format"""
        try:
            # Parse raw data into point cloud
            point_cloud = self._parse_raw_data(raw_data)
            
            # Detect obstacles
            obstacles = await self.obstacle_detector.detect_obstacles(point_cloud)
            
            # Calculate safety metrics
            safety_metrics = self._calculate_safety_metrics(point_cloud, obstacles)
            
            return {
                'timestamp': time.time(),
                'point_count': len(point_cloud),
                'obstacles': obstacles,
                'safety_metrics': safety_metrics,
                'data_quality': self._assess_data_quality(point_cloud)
            }
        except Exception as e:
            logger.error(f"LiDAR data processing error: {e}")
            return None
            
    def _parse_raw_data(self, raw_data: bytes) -> list:
        """Parse raw LiDAR data into point cloud"""
        # Implementation for specific LiDAR format
        # Returns list of (distance, angle) tuples
        pass
        
    def _calculate_safety_metrics(self, point_cloud: list, obstacles: list) -> dict:
        """Calculate safety-related metrics"""
        return {
            'closest_obstacle': min([obs['distance'] for obs in obstacles]) if obstacles else float('inf'),
            'obstacle_count': len(obstacles),
            'clearance_zone': self._calculate_clearance_zone(point_cloud),
            'safety_score': self._calculate_safety_score(point_cloud, obstacles)
        }
```

### **2. Data Processing & Analytics**

#### **Data Filtering:**
```python
class DataFilter:
    def __init__(self):
        self.filters = {}
        
    async def apply_filter(self, data: dict, filter_type: str, 
                          parameters: dict) -> dict:
        """Apply specified filter to data"""
        if filter_type == 'noise_reduction':
            return await self._reduce_noise(data, parameters)
        elif filter_type == 'outlier_detection':
            return await self._detect_outliers(data, parameters)
        elif filter_type == 'smoothing':
            return await self._smooth_data(data, parameters)
        else:
            raise ValueError(f"Unknown filter type: {filter_type}")
            
    async def _reduce_noise(self, data: dict, parameters: dict) -> dict:
        """Reduce noise in sensor data"""
        # Implement noise reduction algorithm
        # e.g., moving average, Kalman filter
        pass
        
    async def _detect_outliers(self, data: dict, parameters: dict) -> dict:
        """Detect and handle outliers in data"""
        # Implement outlier detection
        # e.g., statistical methods, machine learning
        pass
        
    async def _smooth_data(self, data: dict, parameters: dict) -> dict:
        """Smooth noisy data"""
        # Implement data smoothing
        # e.g., exponential smoothing, polynomial fitting
        pass
```

#### **Data Aggregation:**
```python
class DataAggregator:
    def __init__(self, aggregation_window=60):  # 60 seconds
        self.aggregation_window = aggregation_window
        self.data_buffer = {}
        
    async def aggregate_data(self, sensor_id: str, data: dict) -> dict:
        """Aggregate data over time window"""
        if sensor_id not in self.data_buffer:
            self.data_buffer[sensor_id] = []
            
        # Add new data point
        self.data_buffer[sensor_id].append({
            'timestamp': time.time(),
            'data': data
        })
        
        # Remove old data points
        current_time = time.time()
        self.data_buffer[sensor_id] = [
            point for point in self.data_buffer[sensor_id]
            if current_time - point['timestamp'] <= self.aggregation_window
        ]
        
        # Calculate aggregated metrics
        if len(self.data_buffer[sensor_id]) > 0:
            return await self._calculate_aggregated_metrics(sensor_id)
        return None
        
    async def _calculate_aggregated_metrics(self, sensor_id: str) -> dict:
        """Calculate aggregated metrics for sensor data"""
        data_points = self.data_buffer[sensor_id]
        
        # Calculate statistical metrics
        values = [point['data'].get('value', 0) for point in data_points]
        
        return {
            'sensor_id': sensor_id,
            'timestamp': time.time(),
            'data_points': len(data_points),
            'min_value': min(values) if values else 0,
            'max_value': max(values) if values else 0,
            'mean_value': sum(values) / len(values) if values else 0,
            'std_deviation': self._calculate_std_deviation(values) if values else 0
        }
        
    def _calculate_std_deviation(self, values: list) -> float:
        """Calculate standard deviation"""
        if len(values) < 2:
            return 0.0
        mean = sum(values) / len(values)
        variance = sum((x - mean) ** 2 for x in values) / len(values)
        return math.sqrt(variance)
```

### **3. Performance Metrics**

#### **System Performance Monitoring:**
```python
class PerformanceMonitor:
    def __init__(self):
        self.metrics = {}
        self.performance_history = []
        
    async def record_metric(self, metric_name: str, value: float, 
                           tags: dict = None):
        """Record a performance metric"""
        timestamp = time.time()
        metric_data = {
            'name': metric_name,
            'value': value,
            'timestamp': timestamp,
            'tags': tags or {}
        }
        
        # Store in memory
        if metric_name not in self.metrics:
            self.metrics[metric_name] = []
        self.metrics[metric_name].append(metric_data)
        
        # Store in history
        self.performance_history.append(metric_data)
        
        # Check thresholds
        await self._check_thresholds(metric_name, value, tags)
        
    async def get_metric_summary(self, metric_name: str, 
                                time_window: int = 3600) -> dict:
        """Get summary statistics for a metric"""
        current_time = time.time()
        recent_metrics = [
            m for m in self.metrics.get(metric_name, [])
            if current_time - m['timestamp'] <= time_window
        ]
        
        if not recent_metrics:
            return {'error': 'No data available'}
            
        values = [m['value'] for m in recent_metrics]
        
        return {
            'metric_name': metric_name,
            'time_window': time_window,
            'data_points': len(values),
            'min': min(values),
            'max': max(values),
            'mean': sum(values) / len(values),
            'p95': self._calculate_percentile(values, 95),
            'p99': self._calculate_percentile(values, 99)
        }
        
    def _calculate_percentile(self, values: list, percentile: int) -> float:
        """Calculate percentile value"""
        if not values:
            return 0.0
        sorted_values = sorted(values)
        index = (percentile / 100) * (len(sorted_values) - 1)
        if index.is_integer():
            return sorted_values[int(index)]
        else:
            lower = sorted_values[int(index)]
            upper = sorted_values[int(index) + 1]
            return lower + (upper - lower) * (index - int(index))
```

#### **Response Time Monitoring:**
```python
class ResponseTimeMonitor:
    def __init__(self):
        self.response_times = {}
        self.targets = {
            'api_endpoint': 50,      # 50ms
            'database_query': 10,     # 10ms
            'firmware_communication': 20,  # 20ms
            'data_processing': 5      # 5ms
        }
        
    async def start_timing(self, operation_id: str, operation_type: str):
        """Start timing an operation"""
        self.response_times[operation_id] = {
            'type': operation_type,
            'start_time': time.time(),
            'status': 'running'
        }
        
    async def end_timing(self, operation_id: str, status: str = 'completed'):
        """End timing an operation"""
        if operation_id in self.response_times:
            end_time = time.time()
            start_time = self.response_times[operation_id]['start_time']
            response_time = (end_time - start_time) * 1000  # Convert to ms
            
            self.response_times[operation_id].update({
                'end_time': end_time,
                'response_time': response_time,
                'status': status
            })
            
            # Check against target
            operation_type = self.response_times[operation_id]['type']
            target = self.targets.get(operation_type, 100)
            
            if response_time > target:
                logger.warning(f"Operation {operation_id} exceeded target: "
                             f"{response_time:.2f}ms > {target}ms")
                
    async def get_performance_report(self) -> dict:
        """Get performance report for all operations"""
        report = {}
        
        for operation_type in self.targets.keys():
            operations = [
                op for op in self.response_times.values()
                if op['type'] == operation_type and op['status'] == 'completed'
            ]
            
            if operations:
                response_times = [op['response_time'] for op in operations]
                target = self.targets[operation_type]
                
                report[operation_type] = {
                    'target_ms': target,
                    'count': len(operations),
                    'avg_ms': sum(response_times) / len(response_times),
                    'p95_ms': self._calculate_percentile(response_times, 95),
                    'p99_ms': self._calculate_percentile(response_times, 99),
                    'target_met': all(rt <= target for rt in response_times)
                }
                
        return report
```

### **4. Alert System**

#### **Threshold Monitoring:**
```python
class ThresholdMonitor:
    def __init__(self):
        self.thresholds = {}
        self.alert_history = []
        self.alert_handlers = []
        
    async def set_threshold(self, metric_name: str, threshold_type: str,
                           value: float, severity: str = 'warning'):
        """Set a threshold for monitoring"""
        if metric_name not in self.thresholds:
            self.thresholds[metric_name] = []
            
        self.thresholds[metric_name].append({
            'type': threshold_type,  # 'min', 'max', 'rate_of_change'
            'value': value,
            'severity': severity,
            'active': True
        })
        
    async def check_thresholds(self, metric_name: str, current_value: float,
                              previous_value: float = None) -> list:
        """Check if any thresholds are violated"""
        violations = []
        
        if metric_name not in self.thresholds:
            return violations
            
        for threshold in self.thresholds[metric_name]:
            if not threshold['active']:
                continue
                
            violated = False
            if threshold['type'] == 'min' and current_value < threshold['value']:
                violated = True
            elif threshold['type'] == 'max' and current_value > threshold['value']:
                violated = True
            elif threshold['type'] == 'rate_of_change' and previous_value is not None:
                rate = abs(current_value - previous_value)
                if rate > threshold['value']:
                    violated = True
                    
            if violated:
                violation = {
                    'metric_name': metric_name,
                    'threshold': threshold,
                    'current_value': current_value,
                    'timestamp': time.time()
                }
                violations.append(violation)
                
        return violations
        
    async def trigger_alert(self, violation: dict):
        """Trigger an alert for a threshold violation"""
        alert = {
            'id': str(uuid.uuid4()),
            'timestamp': time.time(),
            'severity': violation['threshold']['severity'],
            'message': f"Threshold violation: {violation['metric_name']} = "
                      f"{violation['current_value']} "
                      f"({violation['threshold']['type']} "
                      f"{violation['threshold']['value']})",
            'violation': violation
        }
        
        # Store alert
        self.alert_history.append(alert)
        
        # Send to handlers
        for handler in self.alert_handlers:
            try:
                await handler(alert)
            except Exception as e:
                logger.error(f"Alert handler error: {e}")
                
    async def register_alert_handler(self, handler):
        """Register an alert handler function"""
        self.alert_handlers.append(handler)
```

#### **Notification System:**
```python
class NotificationSystem:
    def __init__(self):
        self.notification_channels = {}
        self.notification_templates = {}
        
    async def register_channel(self, channel_name: str, channel_config: dict):
        """Register a notification channel"""
        self.notification_channels[channel_name] = channel_config
        
    async def send_notification(self, alert: dict, channels: list = None):
        """Send notification through specified channels"""
        if channels is None:
            channels = list(self.notification_channels.keys())
            
        for channel_name in channels:
            if channel_name in self.notification_channels:
                try:
                    await self._send_to_channel(channel_name, alert)
                except Exception as e:
                    logger.error(f"Failed to send notification to {channel_name}: {e}")
                    
    async def _send_to_channel(self, channel_name: str, alert: dict):
        """Send notification to specific channel"""
        channel_config = self.notification_channels[channel_name]
        
        if channel_name == 'email':
            await self._send_email(alert, channel_config)
        elif channel_name == 'webhook':
            await self._send_webhook(alert, channel_config)
        elif channel_name == 'websocket':
            await self._send_websocket(alert, channel_config)
        elif channel_name == 'database':
            await self._store_in_database(alert, channel_config)
            
    async def _send_email(self, alert: dict, config: dict):
        """Send email notification"""
        # Implement email sending logic
        pass
        
    async def _send_webhook(self, alert: dict, config: dict):
        """Send webhook notification"""
        # Implement webhook sending logic
        pass
        
    async def _send_websocket(self, alert: dict, config: dict):
        """Send websocket notification"""
        # Implement websocket notification logic
        pass
        
    async def _store_in_database(self, alert: dict, config: dict):
        """Store alert in database"""
        # Implement database storage logic
        pass
```

---

## 📊 **PERFORMANCE REQUIREMENTS**

### **Latency Targets:**
- **Data Collection**: < 10ms
- **Data Processing**: < 20ms
- **Alert Generation**: < 100ms
- **Notification Delivery**: < 500ms

### **Throughput Targets:**
- **Sensor Data**: 1000+ data points/second
- **LiDAR Processing**: 10+ scans/second
- **Alert Processing**: 100+ alerts/second

### **Reliability Targets:**
- **Data Accuracy**: 99.9%
- **System Uptime**: 99.9%
- **Alert Delivery**: 99.5%

---

## 🧪 **TESTING STRATEGY**

### **Unit Tests:**
```python
class TestTelemetrySystem:
    def test_sensor_data_collection(self):
        """Test sensor data collection"""
        collector = SensorDataCollector()
        collector.register_sensor('test_sensor', 'temperature', {'unit': 'celsius'})
        assert 'test_sensor' in collector.sensors
        
    def test_data_filtering(self):
        """Test data filtering functionality"""
        filter_processor = DataFilter()
        test_data = {'value': 25.5, 'timestamp': time.time()}
        filtered_data = await filter_processor.apply_filter(
            test_data, 'noise_reduction', {'window_size': 5}
        )
        assert filtered_data is not None
        
    def test_threshold_monitoring(self):
        """Test threshold monitoring"""
        monitor = ThresholdMonitor()
        monitor.set_threshold('temperature', 'max', 30.0, 'critical')
        violations = await monitor.check_thresholds('temperature', 35.0)
        assert len(violations) == 1
```

### **Performance Tests:**
```python
class TestTelemetryPerformance:
    async def test_data_processing_latency(self):
        """Test data processing latency"""
        processor = LiDARDataProcessor()
        test_data = b'test_lidar_data'
        
        start_time = time.time()
        result = await processor.process_lidar_data(test_data)
        end_time = time.time()
        
        latency = (end_time - start_time) * 1000
        assert latency < 20, f"Data processing latency {latency}ms exceeds 20ms target"
        
    async def test_alert_generation_speed(self):
        """Test alert generation speed"""
        monitor = ThresholdMonitor()
        monitor.set_threshold('temperature', 'max', 30.0, 'critical')
        
        start_time = time.time()
        violations = await monitor.check_thresholds('temperature', 35.0)
        await monitor.trigger_alert(violations[0])
        end_time = time.time()
        
        response_time = (end_time - start_time) * 1000
        assert response_time < 100, f"Alert generation {response_time}ms exceeds 100ms target"
```

---

## 🔗 **INTEGRATION POINTS**

### **Firmware Integration:**
- **Sensor Data**: Real-time sensor readings
- **LiDAR Data**: Point cloud data
- **Status Updates**: Robot status information
- **Command Feedback**: Motion command responses

### **API Endpoints:**
```python
@router.get("/telemetry/sensors/{sensor_id}")
async def get_sensor_data(sensor_id: str, time_window: int = 3600):
    """Get sensor data for specified time window"""
    pass
    
@router.get("/telemetry/lidar/current")
async def get_current_lidar_data():
    """Get current LiDAR scan data"""
    pass
    
@router.get("/telemetry/performance/metrics")
async def get_performance_metrics():
    """Get system performance metrics"""
    pass
    
@router.get("/telemetry/alerts")
async def get_active_alerts():
    """Get currently active alerts"""
    pass
```

---

## 📚 **REFERENCES**

### **Related Documents:**
- **System Architecture**: `docs/03-ARCHITECTURE/ADR-001-SYSTEM_ARCHITECTURE.md`
- **Performance Standards**: `docs/02-REQUIREMENTS/REQ_BE_01.md`
- **Robot Control**: `docs/05-IMPLEMENTATION/IMP_ROBOT_CONTROL_01.md`
- **Integration Guide**: `docs/05-IMPLEMENTATION/INTEGRATION_GUIDE.md`

### **Technical Standards:**
- **Data Processing**: IEEE 1451 (Smart Transducer Interface)
- **Performance Monitoring**: ISO 9241 (Ergonomic Requirements)
- **Alert Systems**: IEC 60870 (Telecontrol Protocols)

---

## 🚀 **NEXT STEPS**

### **Week 2 Deliverables:**
1. **Data Collection**: Sensor và LiDAR data collection
2. **Basic Processing**: Data filtering và validation
3. **Testing**: Unit tests cho core functionality

### **Week 3 Deliverables:**
1. **Performance Metrics**: System performance monitoring
2. **Alert System**: Threshold monitoring và notifications
3. **Integration**: Firmware data integration

### **Dependencies:**
- Sensor data format defined
- LiDAR communication protocol established
- Database schema for telemetry data
- Alert notification channels configured

---

**📅 Last Updated:** 2025-01-28  
**👥 Author:** Backend Team  
**🎯 Status:** In Development  
**📋 Next Review:** Weekly team meeting
