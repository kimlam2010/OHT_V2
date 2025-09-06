"""
Hybrid Localization Engine for OHT-50 Backend
Combines RFID, Accelerometer, Proximity, and LiDAR data for accurate positioning
"""

import asyncio
import logging
import math
from datetime import datetime, timedelta
from typing import Dict, List, Optional, Tuple, Any
import numpy as np
from sqlalchemy.orm import Session

from app.core.database import get_db
from app.models.map import RFIDTag, RobotPosition
from app.models.telemetry import SensorData
from app.models.sensors import SensorConfiguration

logger = logging.getLogger(__name__)


class HybridLocalizationEngine:
    """Hybrid localization engine combining multiple sensor sources"""
    
    def __init__(self):
        self.map_id: Optional[str] = None
        self.resolution: float = 50.0  # mm per pixel
        self.current_position: Optional[Dict[str, Any]] = None
        self.position_history: List[Dict[str, Any]] = []
        self.sensor_weights = {
            "rfid": 0.4,      # High accuracy, low frequency
            "accelerometer": 0.2,  # Medium accuracy, high frequency
            "proximity": 0.2,      # Medium accuracy, medium frequency
            "lidar": 0.2           # Lower accuracy, high frequency
        }
        self.confidence_threshold = 0.7
        self.max_history_size = 100
        
    async def initialize(self, map_id: str, resolution: float) -> None:
        """
        Initialize localization engine with map
        
        Args:
            map_id: ID of the map to use
            resolution: Map resolution in mm per pixel
        """
        self.map_id = map_id
        self.resolution = resolution
        self.current_position = None
        self.position_history = []
        
        # Load RFID tags for this map
        await self._load_rfid_tags()
        
        logger.info(f"Initialized localization engine for map {map_id}")
    
    async def get_robot_position(self) -> Optional[Dict[str, Any]]:
        """
        Get current robot position using hybrid localization
        
        Returns:
            Robot position with confidence and source
        """
        try:
            # Get sensor data
            sensor_data = await self._get_sensor_data()
            
            if not sensor_data:
                logger.warning("No sensor data available for localization")
                return None
            
            # Process each sensor type
            positions = {}
            confidences = {}
            
            # RFID localization
            if "rfid" in sensor_data:
                rfid_pos = await self._process_rfid_data(sensor_data["rfid"])
                if rfid_pos:
                    positions["rfid"] = rfid_pos
                    confidences["rfid"] = 0.9  # High confidence for RFID
            
            # Accelerometer localization (dead reckoning)
            if "accelerometer" in sensor_data:
                accel_pos = await self._process_accelerometer_data(sensor_data["accelerometer"])
                if accel_pos:
                    positions["accelerometer"] = accel_pos
                    confidences["accelerometer"] = 0.6  # Medium confidence for dead reckoning
            
            # Proximity sensor localization
            if "proximity" in sensor_data:
                prox_pos = await self._process_proximity_data(sensor_data["proximity"])
                if prox_pos:
                    positions["proximity"] = prox_pos
                    confidences["proximity"] = 0.7  # Medium-high confidence
            
            # LiDAR localization
            if "lidar" in sensor_data:
                lidar_pos = await self._process_lidar_data(sensor_data["lidar"])
                if lidar_pos:
                    positions["lidar"] = lidar_pos
                    confidences["lidar"] = 0.5  # Lower confidence for LiDAR
            
            # Fuse positions using weighted average
            if positions:
                fused_position = await self._fuse_positions(positions, confidences)
                
                # Update current position
                self.current_position = fused_position
                
                # Add to history
                self.position_history.append(fused_position)
                if len(self.position_history) > self.max_history_size:
                    self.position_history.pop(0)
                
                return fused_position
            else:
                logger.warning("No valid positions from any sensor")
                return None
                
        except Exception as e:
            logger.error(f"Failed to get robot position: {e}")
            return None
    
    async def _get_sensor_data(self) -> Dict[str, Any]:
        """
        Get recent sensor data from database
        
        Returns:
            Dictionary of sensor data by type
        """
        try:
            db = next(get_db())
            
            # Get data from last 5 seconds
            cutoff_time = datetime.utcnow() - timedelta(seconds=5)
            
            sensor_data = {}
            
            # Get RFID data
            rfid_data = db.query(SensorData).filter(
                SensorData.sensor_type == "rfid",
                SensorData.timestamp >= cutoff_time,
                SensorData.is_valid == True
            ).order_by(desc(SensorData.timestamp)).first()
            
            if rfid_data:
                sensor_data["rfid"] = rfid_data.data
            
            # Get accelerometer data
            accel_data = db.query(SensorData).filter(
                SensorData.sensor_type == "accelerometer",
                SensorData.timestamp >= cutoff_time,
                SensorData.is_valid == True
            ).order_by(desc(SensorData.timestamp)).first()
            
            if accel_data:
                sensor_data["accelerometer"] = accel_data.data
            
            # Get proximity data
            prox_data = db.query(SensorData).filter(
                SensorData.sensor_type == "proximity",
                SensorData.timestamp >= cutoff_time,
                SensorData.is_valid == True
            ).order_by(desc(SensorData.timestamp)).first()
            
            if prox_data:
                sensor_data["proximity"] = prox_data.data
            
            # Get LiDAR data
            lidar_data = db.query(SensorData).filter(
                SensorData.sensor_type == "lidar",
                SensorData.timestamp >= cutoff_time,
                SensorData.is_valid == True
            ).order_by(desc(SensorData.timestamp)).first()
            
            if lidar_data:
                sensor_data["lidar"] = lidar_data.data
            
            return sensor_data
            
        except Exception as e:
            logger.error(f"Failed to get sensor data: {e}")
            return {}
        finally:
            db.close()
    
    async def _process_rfid_data(self, rfid_data: Dict[str, Any]) -> Optional[Dict[str, Any]]:
        """
        Process RFID data for localization
        
        Args:
            rfid_data: RFID sensor data
            
        Returns:
            Robot position from RFID
        """
        try:
            rfid_id = rfid_data.get("rfid_id")
            signal_strength = rfid_data.get("signal_strength", 0)
            
            if not rfid_id or signal_strength < 0.5:  # Minimum signal strength
                return None
            
            # Get RFID tag position from database
            db = next(get_db())
            rfid_tag = db.query(RFIDTag).filter(
                RFIDTag.rfid_id == rfid_id,
                RFIDTag.is_active == True
            ).first()
            
            if rfid_tag:
                # RFID gives exact position
                position = {
                    "x": rfid_tag.position_x,
                    "y": rfid_tag.position_y,
                    "theta": rfid_tag.orientation,
                    "confidence": min(signal_strength, 0.95),
                    "source": "rfid"
                }
                
                logger.debug(f"RFID localization: {position}")
                return position
            
            return None
            
        except Exception as e:
            logger.error(f"Failed to process RFID data: {e}")
            return None
        finally:
            db.close()
    
    async def _process_accelerometer_data(self, accel_data: Dict[str, Any]) -> Optional[Dict[str, Any]]:
        """
        Process accelerometer data for dead reckoning
        
        Args:
            accel_data: Accelerometer sensor data
            
        Returns:
            Robot position from dead reckoning
        """
        try:
            if not self.current_position:
                return None
            
            # Get acceleration values
            ax = accel_data.get("ax", 0)
            ay = accel_data.get("ay", 0)
            az = accel_data.get("az", 0)
            timestamp = accel_data.get("timestamp")
            
            # Simple dead reckoning (integrate acceleration)
            dt = 0.1  # Assume 10Hz update rate
            
            # Update velocity (simplified)
            vx = self.current_position.get("vx", 0) + ax * dt
            vy = self.current_position.get("vy", 0) + ay * dt
            
            # Update position
            x = self.current_position["x"] + vx * dt
            y = self.current_position["y"] + vy * dt
            
            # Update orientation (simplified)
            theta = self.current_position["theta"] + (az * dt * 0.1)  # Scale factor
            
            position = {
                "x": x,
                "y": y,
                "theta": theta,
                "vx": vx,
                "vy": vy,
                "confidence": 0.6,  # Lower confidence for dead reckoning
                "source": "accelerometer"
            }
            
            logger.debug(f"Accelerometer localization: {position}")
            return position
            
        except Exception as e:
            logger.error(f"Failed to process accelerometer data: {e}")
            return None
    
    async def _process_proximity_data(self, prox_data: Dict[str, Any]) -> Optional[Dict[str, Any]]:
        """
        Process proximity sensor data for localization
        
        Args:
            prox_data: Proximity sensor data
            
        Returns:
            Robot position from proximity sensors
        """
        try:
            if not self.current_position:
                return None
            
            # Get proximity readings
            front_distance = prox_data.get("front", 0)
            left_distance = prox_data.get("left", 0)
            right_distance = prox_data.get("right", 0)
            back_distance = prox_data.get("back", 0)
            
            # Use proximity data to refine position
            # This is a simplified approach - in reality, you'd use more sophisticated algorithms
            
            x = self.current_position["x"]
            y = self.current_position["y"]
            theta = self.current_position["theta"]
            
            # Adjust position based on proximity readings
            # This is a placeholder - real implementation would use SLAM or similar
            
            position = {
                "x": x,
                "y": y,
                "theta": theta,
                "confidence": 0.7,
                "source": "proximity"
            }
            
            logger.debug(f"Proximity localization: {position}")
            return position
            
        except Exception as e:
            logger.error(f"Failed to process proximity data: {e}")
            return None
    
    async def _process_lidar_data(self, lidar_data: Dict[str, Any]) -> Optional[Dict[str, Any]]:
        """
        Process LiDAR data for localization
        
        Args:
            lidar_data: LiDAR sensor data
            
        Returns:
            Robot position from LiDAR
        """
        try:
            if not self.current_position:
                return None
            
            # Get LiDAR scan points
            scan_points = lidar_data.get("points", [])
            
            if not scan_points:
                return None
            
            # Simple LiDAR localization using scan matching
            # This is a placeholder - real implementation would use ICP or similar
            
            x = self.current_position["x"]
            y = self.current_position["y"]
            theta = self.current_position["theta"]
            
            # Adjust position based on LiDAR scan
            # This is a simplified approach
            
            position = {
                "x": x,
                "y": y,
                "theta": theta,
                "confidence": 0.5,  # Lower confidence for LiDAR
                "source": "lidar"
            }
            
            logger.debug(f"LiDAR localization: {position}")
            return position
            
        except Exception as e:
            logger.error(f"Failed to process LiDAR data: {e}")
            return None
    
    async def _fuse_positions(self, positions: Dict[str, Dict[str, Any]], 
                            confidences: Dict[str, float]) -> Dict[str, Any]:
        """
        Fuse multiple position estimates using weighted average
        
        Args:
            positions: Dictionary of positions by sensor type
            confidences: Dictionary of confidences by sensor type
            
        Returns:
            Fused position estimate
        """
        try:
            if not positions:
                return None
            
            # Calculate weights based on confidence and sensor type
            total_weight = 0
            weighted_x = 0
            weighted_y = 0
            weighted_theta = 0
            
            for sensor_type, position in positions.items():
                confidence = confidences.get(sensor_type, 0.5)
                sensor_weight = self.sensor_weights.get(sensor_type, 0.1)
                
                # Combined weight
                weight = confidence * sensor_weight
                total_weight += weight
                
                # Weighted sum
                weighted_x += position["x"] * weight
                weighted_y += position["y"] * weight
                weighted_theta += position["theta"] * weight
            
            if total_weight == 0:
                return None
            
            # Calculate weighted average
            fused_x = weighted_x / total_weight
            fused_y = weighted_y / total_weight
            fused_theta = weighted_theta / total_weight
            
            # Calculate overall confidence
            overall_confidence = min(total_weight, 0.95)
            
            # Determine primary source
            primary_source = max(positions.keys(), key=lambda k: confidences.get(k, 0))
            
            fused_position = {
                "x": fused_x,
                "y": fused_y,
                "theta": fused_theta,
                "confidence": overall_confidence,
                "source": "hybrid",
                "primary_source": primary_source,
                "sensor_count": len(positions),
                "timestamp": datetime.utcnow().isoformat()
            }
            
            logger.debug(f"Fused position: {fused_position}")
            return fused_position
            
        except Exception as e:
            logger.error(f"Failed to fuse positions: {e}")
            return None
    
    async def _load_rfid_tags(self) -> None:
        """
        Load RFID tags for current map
        """
        try:
            db = next(get_db())
            rfid_tags = db.query(RFIDTag).filter(RFIDTag.is_active == True).all()
            
            logger.info(f"Loaded {len(rfid_tags)} RFID tags for localization")
            
        except Exception as e:
            logger.error(f"Failed to load RFID tags: {e}")
        finally:
            db.close()
    
    async def finalize(self) -> None:
        """
        Finalize localization engine
        """
        self.map_id = None
        self.current_position = None
        self.position_history = []
        
        logger.info("Localization engine finalized")
    
    def get_position_history(self, limit: int = 10) -> List[Dict[str, Any]]:
        """
        Get position history
        
        Args:
            limit: Maximum number of positions to return
            
        Returns:
            List of recent positions
        """
        return self.position_history[-limit:] if self.position_history else []
    
    async def estimate_position(self, sensor_data: Dict[str, Any]) -> Dict[str, Any]:
        """
        Estimate robot position using hybrid sensor fusion
        
        Args:
            sensor_data: Dictionary containing sensor data
            
        Returns:
            Estimated position with confidence
        """
        try:
            # Simple position estimation (mock implementation)
            # In real implementation, this would use sensor fusion algorithms
            
            # Default position if no sensors available
            x = 0.0
            y = 0.0
            theta = 0.0
            confidence = 0.5
            
            # Process RFID data if available
            if sensor_data.get("rfid"):
                # RFID provides absolute position
                confidence = 0.9
                x = 100.0  # Mock position
                y = 200.0
            
            # Process accelerometer data if available
            if sensor_data.get("accelerometer"):
                accel = sensor_data["accelerometer"]
                # Simple integration (mock)
                x += accel.get("x", 0) * 0.1
                y += accel.get("y", 0) * 0.1
                confidence = max(confidence, 0.7)
            
            # Process LiDAR data if available
            if sensor_data.get("lidar"):
                # LiDAR provides relative position updates
                confidence = max(confidence, 0.6)
            
            position = {
                "x": x,
                "y": y,
                "theta": theta,
                "confidence": confidence,
                "timestamp": datetime.now().isoformat()
            }
            
            # Store in history
            self.position_history.append(position)
            if len(self.position_history) > self.max_history_size:
                self.position_history.pop(0)
            
            self.current_position = position
            return position
            
        except Exception as e:
            logger.error(f"Failed to estimate position: {e}")
            # Return default position on error
            return {
                "x": 0.0,
                "y": 0.0,
                "theta": 0.0,
                "confidence": 0.0,
                "timestamp": datetime.now().isoformat()
            }
    
    def get_localization_stats(self) -> Dict[str, Any]:
        """
        Get localization statistics
        
        Returns:
            Dictionary with localization stats
        """
        if not self.position_history:
            return {
                "total_positions": 0,
                "average_confidence": 0.0,
                "sensor_usage": {}
            }
        
        # Calculate statistics
        total_positions = len(self.position_history)
        average_confidence = sum(p["confidence"] for p in self.position_history) / total_positions
        
        # Count sensor usage
        sensor_usage = {}
        for position in self.position_history:
            source = position.get("primary_source", "unknown")
            sensor_usage[source] = sensor_usage.get(source, 0) + 1
        
        return {
            "total_positions": total_positions,
            "average_confidence": average_confidence,
            "sensor_usage": sensor_usage,
            "current_position": self.current_position
        }
