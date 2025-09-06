"""
Sensor Data Processing Service for OHT-50 Backend
Handles validation, filtering, and processing of sensor data
"""

import logging
import asyncio
from typing import Dict, Any, List, Optional, Tuple
from datetime import datetime, timedelta
from dataclasses import dataclass
from enum import Enum

from app.models.telemetry import SensorData
from app.models.sensors import SensorConfiguration
from app.services.firmware_integration_service import SensorReading, SensorType

logger = logging.getLogger(__name__)


class DataQuality(Enum):
    """Data quality levels"""
    EXCELLENT = "excellent"  # > 0.9
    GOOD = "good"           # 0.7 - 0.9
    FAIR = "fair"           # 0.5 - 0.7
    POOR = "poor"           # 0.3 - 0.5
    BAD = "bad"             # < 0.3


@dataclass
class ProcessingResult:
    """Result of sensor data processing"""
    success: bool
    processed_data: Optional[Dict[str, Any]] = None
    quality_score: float = 0.0
    quality_level: DataQuality = DataQuality.BAD
    errors: List[str] = None
    warnings: List[str] = None
    processing_time: float = 0.0


class SensorDataProcessor:
    """
    Sensor Data Processing Service
    
    Handles validation, filtering, and processing of sensor data
    from multiple sensor types (RFID, Accelerometer, Proximity, LiDAR)
    """
    
    def __init__(self):
        """Initialize sensor data processor"""
        self.validation_rules = self._initialize_validation_rules()
        self.data_history: Dict[str, List[Dict[str, Any]]] = {}
        self.max_history_size = 100
        
        logger.info("🔧 Sensor Data Processor initialized")
    
    def _initialize_validation_rules(self) -> Dict[str, List[Dict[str, Any]]]:
        """Initialize validation rules for each sensor type"""
        return {
            "rfid": [
                {"field": "rfid_id", "required": True, "type": str},
                {"field": "signal_strength", "min": 0.0, "max": 1.0},
                {"field": "distance", "min": 0.0, "max": 1000.0},
                {"field": "angle", "min": -180.0, "max": 180.0}
            ],
            "accelerometer": [
                {"field": "x", "min": -20.0, "max": 20.0},
                {"field": "y", "min": -20.0, "max": 20.0},
                {"field": "z", "min": -20.0, "max": 20.0},
                {"field": "magnitude", "min": 0.0, "max": 30.0}
            ],
            "proximity": [
                {"field": "distance", "min": 0.0, "max": 500.0},
                {"field": "obstacle_detected", "type": bool},
                {"field": "confidence", "min": 0.0, "max": 1.0}
            ],
            "lidar": [
                {"field": "scan_data", "required": True, "type": list},
                {"field": "resolution", "min": 0.1, "max": 10.0},
                {"field": "max_range", "min": 10.0, "max": 1000.0},
                {"field": "min_range", "min": 0.1, "max": 100.0}
            ]
        }
    
    async def process_sensor_data(self, sensor_reading: SensorReading) -> ProcessingResult:
        """
        Process sensor data with validation and filtering
        
        Args:
            sensor_reading: Raw sensor reading
            
        Returns:
            Processing result
        """
        start_time = datetime.utcnow()
        errors = []
        warnings = []
        
        try:
            logger.debug("🔧 Processing sensor data: %s", sensor_reading.sensor_type.value)
            
            # Step 1: Validate data
            validation_result = await self._validate_sensor_data(sensor_reading)
            if not validation_result["valid"]:
                errors.extend(validation_result["errors"])
                warnings.extend(validation_result["warnings"])
            
            # Step 2: Apply filters
            filtered_data = await self._apply_filters(sensor_reading)
            
            # Step 3: Calculate quality score
            quality_score = await self._calculate_quality_score(sensor_reading, filtered_data)
            quality_level = self._get_quality_level(quality_score)
            
            # Step 4: Store in history
            await self._store_in_history(sensor_reading, filtered_data)
            
            # Step 5: Apply additional processing
            processed_data = await self._apply_additional_processing(sensor_reading, filtered_data)
            
            processing_time = (datetime.utcnow() - start_time).total_seconds()
            
            logger.debug("✅ Sensor data processed successfully: quality=%.2f, time=%.3fs", 
                        quality_score, processing_time)
            
            return ProcessingResult(
                success=True,
                processed_data=processed_data,
                quality_score=quality_score,
                quality_level=quality_level,
                errors=errors,
                warnings=warnings,
                processing_time=processing_time
            )
            
        except Exception as e:
            processing_time = (datetime.utcnow() - start_time).total_seconds()
            logger.error("❌ Failed to process sensor data: %s", e)
            
            return ProcessingResult(
                success=False,
                errors=[str(e)],
                processing_time=processing_time
            )
    
    async def _validate_sensor_data(self, sensor_reading: SensorReading) -> Dict[str, Any]:
        """Validate sensor data against rules"""
        sensor_type = sensor_reading.sensor_type.value
        data = sensor_reading.data
        errors = []
        warnings = []
        
        # Get validation rules for sensor type
        rules = self.validation_rules.get(sensor_type, [])
        
        for rule in rules:
            field = rule["field"]
            
            # Check if field exists
            if field not in data:
                if rule.get("required", False):
                    errors.append(f"Required field '{field}' missing")
                continue
            
            value = data[field]
            
            # Check data type
            expected_type = rule.get("type")
            if expected_type and not isinstance(value, expected_type):
                errors.append(f"Field '{field}' has wrong type: expected {expected_type.__name__}, got {type(value).__name__}")
                continue
            
            # Check numeric constraints
            if isinstance(value, (int, float)):
                if "min" in rule and value < rule["min"]:
                    errors.append(f"Field '{field}' value {value} below minimum {rule['min']}")
                if "max" in rule and value > rule["max"]:
                    errors.append(f"Field '{field}' value {value} above maximum {rule['max']}")
            
            # Check string constraints
            if isinstance(value, str) and len(value) == 0:
                errors.append(f"Field '{field}' is empty string")
        
        return {
            "valid": len(errors) == 0,
            "errors": errors,
            "warnings": warnings
        }
    
    async def _apply_filters(self, sensor_reading: SensorReading) -> Dict[str, Any]:
        """Apply filters to sensor data"""
        # Simple filtering implementation
        data = sensor_reading.data.copy()
        
        # Apply basic noise reduction
        for key, value in data.items():
            if isinstance(value, (int, float)):
                # Simple moving average filter
                data[key] = await self._apply_moving_average(key, value)
        
        return data
    
    async def _apply_moving_average(self, key: str, value: float) -> float:
        """Apply moving average filter"""
        history_key = f"filter_{key}"
        
        if history_key not in self.data_history:
            self.data_history[history_key] = []
        
        history = self.data_history[history_key]
        history.append(value)
        
        if len(history) > 5:
            history.pop(0)
        
        return sum(history) / len(history)
    
    async def _calculate_quality_score(self, sensor_reading: SensorReading, filtered_data: Dict[str, Any]) -> float:
        """Calculate quality score for sensor data"""
        try:
            base_quality = sensor_reading.quality
            
            # Calculate additional quality factors
            completeness = self._calculate_completeness(sensor_reading.data)
            consistency = await self._calculate_consistency(sensor_reading)
            
            # Calculate weighted average
            final_quality = (base_quality * 0.6) + (completeness * 0.2) + (consistency * 0.2)
            
            return max(0.0, min(1.0, final_quality))
            
        except Exception as e:
            logger.error("❌ Failed to calculate quality score: %s", e)
            return sensor_reading.quality
    
    def _calculate_completeness(self, data: Dict[str, Any]) -> float:
        """Calculate data completeness score"""
        if not data:
            return 0.0
        
        non_null_count = sum(1 for v in data.values() if v is not None)
        total_count = len(data)
        
        return non_null_count / total_count if total_count > 0 else 0.0
    
    async def _calculate_consistency(self, sensor_reading: SensorReading) -> float:
        """Calculate data consistency score"""
        try:
            sensor_type = sensor_reading.sensor_type.value
            data = sensor_reading.data
            
            if sensor_type == "accelerometer":
                x, y, z = data.get("x", 0), data.get("y", 0), data.get("z", 0)
                magnitude = (x**2 + y**2 + z**2)**0.5
                expected_magnitude = data.get("magnitude", magnitude)
                
                if abs(magnitude - expected_magnitude) < 0.1:
                    return 1.0
                else:
                    return 0.8
            
            elif sensor_type == "lidar":
                scan_data = data.get("scan_data", [])
                if len(scan_data) > 0:
                    valid_ranges = sum(1 for d in scan_data if 0 <= d <= 1000)
                    return valid_ranges / len(scan_data)
                else:
                    return 0.0
            
            else:
                return 0.9
            
        except Exception as e:
            logger.error("❌ Failed to calculate consistency: %s", e)
            return 0.5
    
    def _get_quality_level(self, quality_score: float) -> DataQuality:
        """Get quality level from score"""
        if quality_score >= 0.9:
            return DataQuality.EXCELLENT
        elif quality_score >= 0.7:
            return DataQuality.GOOD
        elif quality_score >= 0.5:
            return DataQuality.FAIR
        elif quality_score >= 0.3:
            return DataQuality.POOR
        else:
            return DataQuality.BAD
    
    async def _store_in_history(self, sensor_reading: SensorReading, processed_data: Dict[str, Any]):
        """Store processed data in history"""
        try:
            sensor_id = sensor_reading.sensor_id
            history_key = f"processed_{sensor_id}"
            
            if history_key not in self.data_history:
                self.data_history[history_key] = []
            
            self.data_history[history_key].append({
                "timestamp": sensor_reading.timestamp,
                "original_data": sensor_reading.data,
                "processed_data": processed_data,
                "quality": sensor_reading.quality
            })
            
            # Limit history size
            if len(self.data_history[history_key]) > self.max_history_size:
                self.data_history[history_key].pop(0)
                
        except Exception as e:
            logger.error("❌ Failed to store in history: %s", e)
    
    async def _apply_additional_processing(self, sensor_reading: SensorReading, filtered_data: Dict[str, Any]) -> Dict[str, Any]:
        """Apply additional sensor-specific processing"""
        try:
            sensor_type = sensor_reading.sensor_type.value
            processed_data = filtered_data.copy()
            
            if sensor_type == "rfid":
                # Calculate position from RFID data
                signal_strength = processed_data.get("signal_strength", 0)
                if signal_strength > 0.5:
                    processed_data["position_confidence"] = signal_strength
            
            elif sensor_type == "accelerometer":
                # Detect motion
                magnitude = processed_data.get("magnitude", 0)
                is_moving = abs(magnitude - 9.8) > 0.5
                processed_data["is_moving"] = is_moving
            
            elif sensor_type == "proximity":
                # Enhanced obstacle detection
                distance = processed_data.get("distance", 0)
                confidence = processed_data.get("confidence", 0)
                is_obstacle = distance < 50.0 and confidence > 0.7
                processed_data["is_obstacle"] = is_obstacle
            
            elif sensor_type == "lidar":
                # Process LiDAR data for mapping
                scan_data = processed_data.get("scan_data", [])
                if len(scan_data) > 0:
                    obstacles = [i for i, d in enumerate(scan_data) if d < 100.0]
                    processed_data["obstacle_count"] = len(obstacles)
            
            return processed_data
            
        except Exception as e:
            logger.error("❌ Failed to apply additional processing: %s", e)
            return filtered_data
    
    async def get_processing_stats(self) -> Dict[str, Any]:
        """Get processing statistics"""
        try:
            total_processed = sum(len(history) for history in self.data_history.values())
            
            return {
                "total_processed": total_processed,
                "history_size": len(self.data_history),
                "max_history_size": self.max_history_size,
                "sensor_types": list(self.validation_rules.keys())
            }
            
        except Exception as e:
            logger.error("❌ Failed to get processing stats: %s", e)
            return {}


# Global sensor data processor instance
sensor_processor = SensorDataProcessor()