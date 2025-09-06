"""
Safety Speed Control Integration for OHT-50
Integrates speed control with safety system and LiDAR obstacle detection
"""

import logging
from typing import Dict, Any, Optional, List
from datetime import datetime, timezone
from dataclasses import dataclass
from enum import Enum

from app.services.speed_control import speed_controller, SpeedMode
from app.services.safety import SafetyService

logger = logging.getLogger(__name__)


class ObstacleType(Enum):
    """Obstacle types from LiDAR detection"""
    STATIC = "static"
    DYNAMIC = "dynamic"
    PERSON = "person"
    VEHICLE = "vehicle"
    UNKNOWN = "unknown"


@dataclass
class Obstacle:
    """Obstacle information from LiDAR"""
    distance: float  # meters
    angle: float  # degrees
    type: ObstacleType
    confidence: float  # 0.0 to 1.0
    timestamp: datetime


class SafetySpeedControl:
    """
    Safety-integrated speed control system
    Integrates with LiDAR obstacle detection and safety system
    """
    
    def __init__(self):
        self.safety_service = SafetyService()
        self.obstacles: List[Obstacle] = []
        self.safety_zones = {
            "critical": {"distance": 1.0, "speed_limit": 0.1},  # 1m, 0.1 m/s
            "warning": {"distance": 3.0, "speed_limit": 0.5},   # 3m, 0.5 m/s
            "caution": {"distance": 5.0, "speed_limit": 1.0}    # 5m, 1.0 m/s
        }
        self.last_obstacle_update = datetime.now(timezone.utc)
        self.emergency_stop_active = False
        
        logger.info("SafetySpeedControl initialized")
    
    async def update_obstacles(self, obstacle_data: List[Dict[str, Any]]) -> None:
        """
        Update obstacle information from LiDAR
        This would be called by the LiDAR processing system
        """
        try:
            self.obstacles.clear()
            
            for obs_data in obstacle_data:
                obstacle = Obstacle(
                    distance=obs_data.get("distance", 0.0),
                    angle=obs_data.get("angle", 0.0),
                    type=ObstacleType(obs_data.get("type", "unknown")),
                    confidence=obs_data.get("confidence", 0.0),
                    timestamp=datetime.now(timezone.utc)
                )
                self.obstacles.append(obstacle)
            
            self.last_obstacle_update = datetime.now(timezone.utc)
            
            # Check for safety violations
            await self._check_safety_violations()
            
            logger.debug("Updated %d obstacles", len(self.obstacles))
            
        except Exception as e:
            logger.error("Failed to update obstacles: %s", e)
    
    async def _check_safety_violations(self) -> None:
        """Check for safety violations and adjust speed accordingly"""
        try:
            # Find closest obstacle
            closest_obstacle = self._find_closest_obstacle()
            
            if closest_obstacle:
                # Determine safety zone
                safety_zone = self._determine_safety_zone(closest_obstacle.distance)
                
                if safety_zone:
                    # Check if current speed exceeds safety limit
                    current_speed = speed_controller.current_speed
                    speed_limit = safety_zone["speed_limit"]
                    
                    if current_speed > speed_limit:
                        logger.warning("Speed violation detected: %.2f m/s > %.2f m/s limit", 
                                     current_speed, speed_limit)
                        
                        # Reduce speed to safety limit
                        await self._enforce_safety_speed(speed_limit, closest_obstacle)
                
                # Check for emergency stop conditions
                if closest_obstacle.distance < self.safety_zones["critical"]["distance"]:
                    if closest_obstacle.type in [ObstacleType.PERSON, ObstacleType.VEHICLE]:
                        logger.critical("Emergency stop: Person/vehicle detected at %.2f m", 
                                      closest_obstacle.distance)
                        await self._trigger_emergency_stop()
            
        except Exception as e:
            logger.error("Safety violation check failed: %s", e)
    
    def _find_closest_obstacle(self) -> Optional[Obstacle]:
        """Find the closest obstacle"""
        if not self.obstacles:
            return None
        
        return min(self.obstacles, key=lambda obs: obs.distance)
    
    def _determine_safety_zone(self, distance: float) -> Optional[Dict[str, Any]]:
        """Determine which safety zone the distance falls into"""
        if distance <= self.safety_zones["critical"]["distance"]:
            return self.safety_zones["critical"]
        elif distance <= self.safety_zones["warning"]["distance"]:
            return self.safety_zones["warning"]
        elif distance <= self.safety_zones["caution"]["distance"]:
            return self.safety_zones["caution"]
        else:
            return None
    
    async def _enforce_safety_speed(self, speed_limit: float, obstacle: Obstacle) -> None:
        """Enforce safety speed limit"""
        try:
            logger.info("Enforcing safety speed: %.2f m/s due to obstacle at %.2f m", 
                       speed_limit, obstacle.distance)
            
            # Set safety mode and reduce speed
            result = await speed_controller.set_speed(
                target_speed=speed_limit,
                mode=SpeedMode.SAFETY,
                safety_override=True
            )
            
            if result["success"]:
                # Log safety event
                await self.safety_service.log_safety_event(
                    event_type="speed_reduction",
                    description=f"Speed reduced to {speed_limit} m/s due to obstacle at {obstacle.distance:.2f}m",
                    severity="warning"
                )
            else:
                logger.error("Failed to enforce safety speed: %s", result.get("error"))
                
        except Exception as e:
            logger.error("Failed to enforce safety speed: %s", e)
    
    async def _trigger_emergency_stop(self) -> None:
        """Trigger emergency stop"""
        try:
            if not self.emergency_stop_active:
                self.emergency_stop_active = True
                
                logger.critical("Triggering emergency stop due to safety violation")
                
                # Execute emergency stop
                result = await speed_controller.emergency_stop()
                
                if result["success"]:
                    # Log emergency event
                    await self.safety_service.log_safety_event(
                        event_type="emergency_stop",
                        severity="critical",
                        message="Emergency stop triggered due to safety violation"
                    )
                    
                    # Notify safety system
                    await self.safety_service.trigger_emergency_stop()
                else:
                    logger.error("Emergency stop failed: %s", result.get("error"))
                
        except Exception as e:
            logger.error("Failed to trigger emergency stop: %s", e)
    
    async def get_safety_status(self) -> Dict[str, Any]:
        """Get current safety status"""
        try:
            closest_obstacle = self._find_closest_obstacle()
            
            return {
                "obstacles_detected": len(self.obstacles),
                "closest_obstacle": {
                    "distance": closest_obstacle.distance if closest_obstacle else None,
                    "angle": closest_obstacle.angle if closest_obstacle else None,
                    "type": closest_obstacle.type.value if closest_obstacle else None,
                    "confidence": closest_obstacle.confidence if closest_obstacle else None
                } if closest_obstacle else None,
                "safety_zones": self.safety_zones,
                "emergency_stop_active": self.emergency_stop_active,
                "last_obstacle_update": self.last_obstacle_update.isoformat(),
                "current_speed": speed_controller.current_speed,
                "safety_active": speed_controller.safety_active
            }
            
        except Exception as e:
            logger.error("Failed to get safety status: %s", e)
            return {"error": str(e)}
    
    async def reset_emergency_stop(self) -> bool:
        """Reset emergency stop state"""
        try:
            if self.emergency_stop_active:
                self.emergency_stop_active = False
                logger.info("Emergency stop state reset")
                
                # Log reset event
                await self.safety_service.log_safety_event(
                    event_type="emergency_reset",
                    description="Emergency stop state reset",
                    severity="info"
                )
                
                return True
            else:
                logger.warning("Emergency stop reset requested but not active")
                return False
                
        except Exception as e:
            logger.error("Failed to reset emergency stop: %s", e)
            return False
    
    def configure_safety_zones(self, zones: Dict[str, Dict[str, float]]) -> None:
        """Configure safety zones"""
        try:
            self.safety_zones.update(zones)
            logger.info("Safety zones updated: %s", self.safety_zones)
        except Exception as e:
            logger.error("Failed to configure safety zones: %s", e)


# Global safety speed control instance
safety_speed_control = SafetySpeedControl()
