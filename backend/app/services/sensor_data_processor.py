"""
Sensor Data Processor for OHT-50
Real-time processing of RFID, accelerometer, compass, and docking sensors
"""

import asyncio
import logging
import math
import time
from typing import Dict, Any, List, Optional, Tuple
from datetime import datetime, timezone
from dataclasses import dataclass, field
from enum import Enum
import statistics

logger = logging.getLogger(__name__)


class SensorType(Enum):
    """Sensor types"""
    RFID = "rfid"
    ACCELEROMETER = "accelerometer"
    COMPASS = "compass"
    DOCKING = "docking"
    MOTOR = "motor"
    BATTERY = "battery"
    TEMPERATURE = "temperature"


class SensorStatus(Enum):
    """Sensor status"""
    ACTIVE = "active"
    INACTIVE = "inactive"
    ERROR = "error"
    CALIBRATING = "calibrating"


@dataclass
class RFIDData:
    """RFID sensor data"""
    tag_id: str
    signal_strength: float  # 0.0 to 100.0
    distance: float  # estimated distance in meters
    timestamp: datetime = field(default_factory=lambda: datetime.now(timezone.utc))


@dataclass
class AccelerometerData:
    """Accelerometer sensor data"""
    x: float  # m/s²
    y: float  # m/s²
    z: float  # m/s²
    magnitude: float  # total acceleration
    timestamp: datetime = field(default_factory=lambda: datetime.now(timezone.utc))


@dataclass
class CompassData:
    """Compass sensor data"""
    heading: float  # degrees (0-360)
    magnetic_field_x: float
    magnetic_field_y: float
    magnetic_field_z: float
    accuracy: float  # 0.0 to 1.0
    timestamp: datetime = field(default_factory=lambda: datetime.now(timezone.utc))


@dataclass
class DockingData:
    """Docking sensor data"""
    is_docked: bool
    dock_id: Optional[str]
    alignment_angle: float  # degrees
    proximity_sensors: List[float]  # 4 proximity sensor readings
    timestamp: datetime = field(default_factory=lambda: datetime.now(timezone.utc))


@dataclass
class ProcessedSensorData:
    """Processed sensor data with analysis"""
    sensor_type: SensorType
    raw_data: Dict[str, Any]
    processed_data: Any
    quality_score: float  # 0.0 to 1.0
    status: SensorStatus
    processing_time_ms: float
    timestamp: datetime = field(default_factory=lambda: datetime.now(timezone.utc))


class SensorDataProcessor:
    """
    High-performance sensor data processor
    Target: < 20ms processing time for all sensors
    """
    
    def __init__(self):
        # Processing parameters
        self.rfid_signal_threshold = 30.0  # minimum signal strength
        self.acceleration_threshold = 0.5  # m/s² for movement detection
        self.compass_accuracy_threshold = 0.8  # minimum accuracy
        self.docking_proximity_threshold = 0.1  # meters
        
        # Calibration data
        self.accelerometer_calibration = {"x": 0.0, "y": 0.0, "z": 9.81}  # gravity compensation
        self.compass_calibration = {"offset": 0.0, "scale": 1.0}
        
        # Data history for analysis
        self.data_history: Dict[SensorType, List[Any]] = {
            SensorType.RFID: [],
            SensorType.ACCELEROMETER: [],
            SensorType.COMPASS: [],
            SensorType.DOCKING: []
        }
        
        # Performance tracking
        self.processing_times: Dict[SensorType, List[float]] = {
            SensorType.RFID: [],
            SensorType.ACCELEROMETER: [],
            SensorType.COMPASS: [],
            SensorType.DOCKING: []
        }
        
        # Sensor status tracking
        self.sensor_status: Dict[SensorType, SensorStatus] = {
            SensorType.RFID: SensorStatus.ACTIVE,
            SensorType.ACCELEROMETER: SensorStatus.ACTIVE,
            SensorType.COMPASS: SensorStatus.ACTIVE,
            SensorType.DOCKING: SensorStatus.ACTIVE
        }
        
        logger.info("SensorDataProcessor initialized")
    
    async def process_sensor_data(self, sensor_type: SensorType, raw_data: Dict[str, Any]) -> ProcessedSensorData:
        """
        Process sensor data based on type
        Target: < 20ms processing time
        """
        start_time = time.perf_counter()
        
        try:
            # Route to appropriate processor
            if sensor_type == SensorType.RFID:
                processed_data = await self._process_rfid_data(raw_data)
            elif sensor_type == SensorType.ACCELEROMETER:
                processed_data = await self._process_accelerometer_data(raw_data)
            elif sensor_type == SensorType.COMPASS:
                processed_data = await self._process_compass_data(raw_data)
            elif sensor_type == SensorType.DOCKING:
                processed_data = await self._process_docking_data(raw_data)
            else:
                processed_data = raw_data  # Pass through for unknown types
            
            # Calculate quality score
            quality_score = self._calculate_quality_score(sensor_type, raw_data, processed_data)
            
            # Determine sensor status
            status = self._determine_sensor_status(sensor_type, quality_score)
            
            # Create processed data
            processing_time = (time.perf_counter() - start_time) * 1000
            result = ProcessedSensorData(
                sensor_type=sensor_type,
                raw_data=raw_data,
                processed_data=processed_data,
                quality_score=quality_score,
                status=status,
                processing_time_ms=processing_time
            )
            
            # Update history and statistics
            self._update_history(sensor_type, processed_data)
            self._update_processing_time(sensor_type, processing_time)
            
            logger.debug("Processed %s data: quality=%.2f, time=%.2fms", 
                        sensor_type.value, quality_score, processing_time)
            
            return result
            
        except Exception as e:
            logger.error("Error processing %s data: %s", sensor_type.value, e)
            # Return error result
            return ProcessedSensorData(
                sensor_type=sensor_type,
                raw_data=raw_data,
                processed_data=raw_data,
                quality_score=0.0,
                status=SensorStatus.ERROR,
                processing_time_ms=(time.perf_counter() - start_time) * 1000
            )
    
    async def _process_rfid_data(self, raw_data: Dict[str, Any]) -> RFIDData:
        """Process RFID sensor data"""
        try:
            tag_id = raw_data.get("tag_id", "unknown")
            signal_strength = float(raw_data.get("signal_strength", 0.0))
            distance = float(raw_data.get("distance", 0.0))
            
            # Estimate distance from signal strength if not provided
            if distance == 0.0 and signal_strength > 0:
                distance = self._estimate_rfid_distance(signal_strength)
            
            return RFIDData(
                tag_id=tag_id,
                signal_strength=signal_strength,
                distance=distance
            )
            
        except Exception as e:
            logger.error("Error processing RFID data: %s", e)
            return RFIDData(tag_id="error", signal_strength=0.0, distance=0.0)
    
    def _estimate_rfid_distance(self, signal_strength: float) -> float:
        """Estimate distance from RFID signal strength"""
        # Simple inverse relationship (in real implementation, use proper calibration)
        if signal_strength <= 0:
            return 10.0  # Maximum range
        
        # Estimate based on signal strength (empirical formula)
        distance = 10.0 * (1.0 - signal_strength / 100.0)
        return max(0.1, min(10.0, distance))
    
    async def _process_accelerometer_data(self, raw_data: Dict[str, Any]) -> AccelerometerData:
        """Process accelerometer sensor data"""
        try:
            x = float(raw_data.get("x", 0.0))
            y = float(raw_data.get("y", 0.0))
            z = float(raw_data.get("z", 0.0))
            
            # Apply calibration
            x -= self.accelerometer_calibration["x"]
            y -= self.accelerometer_calibration["y"]
            z -= self.accelerometer_calibration["z"]
            
            # Calculate magnitude
            magnitude = math.sqrt(x**2 + y**2 + z**2)
            
            return AccelerometerData(
                x=x,
                y=y,
                z=z,
                magnitude=magnitude
            )
            
        except Exception as e:
            logger.error("Error processing accelerometer data: %s", e)
            return AccelerometerData(x=0.0, y=0.0, z=0.0, magnitude=0.0)
    
    async def _process_compass_data(self, raw_data: Dict[str, Any]) -> CompassData:
        """Process compass sensor data"""
        try:
            heading = float(raw_data.get("heading", 0.0))
            magnetic_field_x = float(raw_data.get("magnetic_field_x", 0.0))
            magnetic_field_y = float(raw_data.get("magnetic_field_y", 0.0))
            magnetic_field_z = float(raw_data.get("magnetic_field_z", 0.0))
            accuracy = float(raw_data.get("accuracy", 1.0))
            
            # Apply calibration
            heading = (heading + self.compass_calibration["offset"]) * self.compass_calibration["scale"]
            heading = heading % 360.0  # Normalize to 0-360
            
            return CompassData(
                heading=heading,
                magnetic_field_x=magnetic_field_x,
                magnetic_field_y=magnetic_field_y,
                magnetic_field_z=magnetic_field_z,
                accuracy=accuracy
            )
            
        except Exception as e:
            logger.error("Error processing compass data: %s", e)
            return CompassData(heading=0.0, magnetic_field_x=0.0, magnetic_field_y=0.0, 
                             magnetic_field_z=0.0, accuracy=0.0)
    
    async def _process_docking_data(self, raw_data: Dict[str, Any]) -> DockingData:
        """Process docking sensor data"""
        try:
            is_docked = bool(raw_data.get("is_docked", False))
            dock_id = raw_data.get("dock_id")
            alignment_angle = float(raw_data.get("alignment_angle", 0.0))
            proximity_sensors = raw_data.get("proximity_sensors", [0.0, 0.0, 0.0, 0.0])
            
            # Ensure proximity_sensors is a list of 4 floats
            if not isinstance(proximity_sensors, list):
                proximity_sensors = [0.0, 0.0, 0.0, 0.0]
            
            # Pad or truncate to 4 sensors
            while len(proximity_sensors) < 4:
                proximity_sensors.append(0.0)
            proximity_sensors = proximity_sensors[:4]
            
            # Convert to floats
            proximity_sensors = [float(sensor) for sensor in proximity_sensors]
            
            return DockingData(
                is_docked=is_docked,
                dock_id=dock_id,
                alignment_angle=alignment_angle,
                proximity_sensors=proximity_sensors
            )
            
        except Exception as e:
            logger.error("Error processing docking data: %s", e)
            return DockingData(is_docked=False, dock_id=None, alignment_angle=0.0, 
                             proximity_sensors=[0.0, 0.0, 0.0, 0.0])
    
    def _calculate_quality_score(self, sensor_type: SensorType, raw_data: Dict[str, Any], 
                                processed_data: Any) -> float:
        """Calculate data quality score"""
        try:
            if sensor_type == SensorType.RFID:
                return self._calculate_rfid_quality(processed_data)
            elif sensor_type == SensorType.ACCELEROMETER:
                return self._calculate_accelerometer_quality(processed_data)
            elif sensor_type == SensorType.COMPASS:
                return self._calculate_compass_quality(processed_data)
            elif sensor_type == SensorType.DOCKING:
                return self._calculate_docking_quality(processed_data)
            else:
                return 0.5  # Default quality
                
        except Exception as e:
            logger.error("Error calculating quality score: %s", e)
            return 0.0
    
    def _calculate_rfid_quality(self, rfid_data: RFIDData) -> float:
        """Calculate RFID data quality"""
        # Quality based on signal strength
        if rfid_data.signal_strength >= 80:
            return 1.0
        elif rfid_data.signal_strength >= 60:
            return 0.8
        elif rfid_data.signal_strength >= 40:
            return 0.6
        elif rfid_data.signal_strength >= 20:
            return 0.4
        else:
            return 0.2
    
    def _calculate_accelerometer_quality(self, accel_data: AccelerometerData) -> float:
        """Calculate accelerometer data quality"""
        # Quality based on magnitude (should be close to 9.81 m/s² when stationary)
        expected_magnitude = 9.81
        deviation = abs(accel_data.magnitude - expected_magnitude)
        
        if deviation <= 0.5:
            return 1.0
        elif deviation <= 1.0:
            return 0.8
        elif deviation <= 2.0:
            return 0.6
        elif deviation <= 5.0:
            return 0.4
        else:
            return 0.2
    
    def _calculate_compass_quality(self, compass_data: CompassData) -> float:
        """Calculate compass data quality"""
        # Quality based on accuracy and magnetic field strength
        accuracy_score = compass_data.accuracy
        
        # Check magnetic field strength
        field_strength = math.sqrt(
            compass_data.magnetic_field_x**2 + 
            compass_data.magnetic_field_y**2 + 
            compass_data.magnetic_field_z**2
        )
        
        # Normal magnetic field strength is around 25-65 microtesla
        if 20 <= field_strength <= 70:
            field_score = 1.0
        elif 15 <= field_strength <= 80:
            field_score = 0.8
        else:
            field_score = 0.5
        
        return (accuracy_score + field_score) / 2.0
    
    def _calculate_docking_quality(self, docking_data: DockingData) -> float:
        """Calculate docking data quality"""
        # Quality based on proximity sensor readings
        if not docking_data.proximity_sensors:
            return 0.0
        
        # Check for reasonable proximity readings
        valid_readings = 0
        for reading in docking_data.proximity_sensors:
            if 0.0 <= reading <= 2.0:  # Reasonable range
                valid_readings += 1
        
        return valid_readings / len(docking_data.proximity_sensors)
    
    def _determine_sensor_status(self, sensor_type: SensorType, quality_score: float) -> SensorStatus:
        """Determine sensor status based on quality"""
        if quality_score >= 0.8:
            return SensorStatus.ACTIVE
        elif quality_score >= 0.5:
            return SensorStatus.ACTIVE  # Still functional
        elif quality_score >= 0.2:
            return SensorStatus.INACTIVE
        else:
            return SensorStatus.ERROR
    
    def _update_history(self, sensor_type: SensorType, processed_data: Any):
        """Update data history for analysis"""
        history = self.data_history[sensor_type]
        history.append(processed_data)
        
        # Keep only recent history (last 100 readings)
        if len(history) > 100:
            history.pop(0)
    
    def _update_processing_time(self, sensor_type: SensorType, processing_time: float):
        """Update processing time statistics"""
        times = self.processing_times[sensor_type]
        times.append(processing_time)
        
        # Keep only recent times (last 100)
        if len(times) > 100:
            times.pop(0)
    
    def get_sensor_statistics(self) -> Dict[str, Any]:
        """Get sensor processing statistics"""
        stats = {}
        
        for sensor_type in SensorType:
            times = self.processing_times.get(sensor_type, [])
            history = self.data_history.get(sensor_type, [])
            
            if times:
                avg_time = statistics.mean(times)
                max_time = max(times)
                min_time = min(times)
            else:
                avg_time = max_time = min_time = 0.0
            
            stats[sensor_type.value] = {
                "status": self.sensor_status[sensor_type].value,
                "data_points": len(history),
                "avg_processing_time_ms": avg_time,
                "max_processing_time_ms": max_time,
                "min_processing_time_ms": min_time
            }
        
        return stats
    
    def get_sensor_history(self, sensor_type: SensorType, limit: int = 50) -> List[Any]:
        """Get sensor data history"""
        history = self.data_history.get(sensor_type, [])
        return history[-limit:] if history else []
    
    def calibrate_sensor(self, sensor_type: SensorType, calibration_data: Dict[str, Any]) -> bool:
        """Calibrate sensor with provided data"""
        try:
            if sensor_type == SensorType.ACCELEROMETER:
                self.accelerometer_calibration.update(calibration_data)
                logger.info("Accelerometer calibrated: %s", calibration_data)
            elif sensor_type == SensorType.COMPASS:
                self.compass_calibration.update(calibration_data)
                logger.info("Compass calibrated: %s", calibration_data)
            else:
                logger.warning("Calibration not supported for sensor type: %s", sensor_type.value)
                return False
            
            return True
            
        except Exception as e:
            logger.error("Error calibrating sensor %s: %s", sensor_type.value, e)
            return False
    
    def detect_movement(self, threshold: float = None) -> bool:
        """Detect robot movement from accelerometer data"""
        if threshold is None:
            threshold = self.acceleration_threshold
        
        accel_history = self.data_history.get(SensorType.ACCELEROMETER, [])
        
        if len(accel_history) < 2:
            return False
        
        # Check recent accelerometer readings
        recent_readings = accel_history[-5:]  # Last 5 readings
        
        for reading in recent_readings:
            if hasattr(reading, 'magnitude') and reading.magnitude > threshold:
                return True
        
        return False
    
    def get_current_heading(self) -> Optional[float]:
        """Get current heading from compass data"""
        compass_history = self.data_history.get(SensorType.COMPASS, [])
        
        if not compass_history:
            return None
        
        latest_reading = compass_history[-1]
        if hasattr(latest_reading, 'heading'):
            return latest_reading.heading
        
        return None
    
    def is_docked(self) -> bool:
        """Check if robot is currently docked"""
        docking_history = self.data_history.get(SensorType.DOCKING, [])
        
        if not docking_history:
            return False
        
        latest_reading = docking_history[-1]
        if hasattr(latest_reading, 'is_docked'):
            return latest_reading.is_docked
        
        return False
    
    def clear_history(self):
        """Clear all sensor data history"""
        for sensor_type in SensorType:
            self.data_history[sensor_type].clear()
            self.processing_times[sensor_type].clear()
        
        logger.info("Cleared all sensor data history")


# Global sensor data processor instance
sensor_data_processor = SensorDataProcessor()
