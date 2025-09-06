"""
Robot Control System Integration for OHT-50
Integrates motion planning, position control, and speed control
"""

import asyncio
import logging
from typing import Dict, Any, Optional, List, Tuple
from datetime import datetime, timezone
from dataclasses import dataclass
from enum import Enum

from app.services.speed_control import speed_controller, SpeedMode
from app.services.safety_speed_control import safety_speed_control
from app.services.robot_control import RobotControlService
from app.services.safety import SafetyService

logger = logging.getLogger(__name__)


class RobotState(Enum):
    """Robot operational states"""
    IDLE = "idle"
    MOVING = "moving"
    DOCKING = "docking"
    CHARGING = "charging"
    ERROR = "error"
    EMERGENCY_STOP = "emergency_stop"
    MAINTENANCE = "maintenance"


class MovementMode(Enum):
    """Movement control modes"""
    MANUAL = "manual"
    AUTO = "auto"
    SEMI_AUTO = "semi_auto"
    MAINTENANCE = "maintenance"


@dataclass
class Position:
    """Robot position in 2D space"""
    x: float  # meters
    y: float  # meters
    theta: float  # radians (heading angle)


@dataclass
class MovementCommand:
    """Movement command structure"""
    target_position: Position
    target_speed: float
    movement_mode: MovementMode
    safety_override: bool = False
    timestamp: datetime = None
    
    def __post_init__(self):
        if self.timestamp is None:
            self.timestamp = datetime.now(timezone.utc)


class RobotControlSystem:
    """
    Integrated robot control system
    Combines motion planning, position control, and speed control
    """
    
    def __init__(self):
        self.current_state = RobotState.IDLE
        self.current_position = Position(0.0, 0.0, 0.0)
        self.target_position = Position(0.0, 0.0, 0.0)
        self.movement_mode = MovementMode.MANUAL
        
        # Service integrations
        self.robot_control_service = RobotControlService()
        self.safety_service = SafetyService()
        
        # System status
        self.last_update = datetime.now(timezone.utc)
        self.error_count = 0
        self.operation_count = 0
        
        # Performance tracking
        self.response_times: List[float] = []
        self.command_history: List[MovementCommand] = []
        
        logger.info("RobotControlSystem initialized")
    
    async def move_to_position(self, target_position: Position, 
                              target_speed: float = 1.0,
                              movement_mode: MovementMode = MovementMode.AUTO,
                              safety_override: bool = False) -> Dict[str, Any]:
        """
        Move robot to target position with integrated control
        """
        start_time = datetime.now(timezone.utc)
        
        try:
            # Validate input
            if not self._validate_movement_input(target_position, target_speed, movement_mode):
                self.error_count += 1
                return {
                    "success": False,
                    "error": "Invalid movement input",
                    "current_position": self._position_to_dict(self.current_position),
                    "target_position": self._position_to_dict(target_position)
                }
            
            # Check safety conditions
            if not safety_override and not await self._check_safety_conditions():
                self.error_count += 1
                return {
                    "success": False,
                    "error": "Safety conditions not met",
                    "current_position": self._position_to_dict(self.current_position),
                    "target_position": self._position_to_dict(target_position)
                }
            
            # Create movement command
            command = MovementCommand(
                target_position=target_position,
                target_speed=target_speed,
                movement_mode=movement_mode,
                safety_override=safety_override
            )
            
            # Execute movement
            success = await self._execute_movement(command)
            
            # Calculate response time
            response_time = (datetime.now(timezone.utc) - start_time).total_seconds() * 1000
            self.response_times.append(response_time)
            self.operation_count += 1
            
            # Log performance
            if response_time > 50.0:  # Target: < 50ms
                logger.warning("Movement response time exceeded target: %.2f ms", response_time)
            
            return {
                "success": success,
                "current_position": self._position_to_dict(self.current_position),
                "target_position": self._position_to_dict(self.target_position),
                "state": self.current_state.value,
                "movement_mode": self.movement_mode.value,
                "response_time_ms": response_time,
                "timestamp": start_time.isoformat()
            }
            
        except Exception as e:
            logger.error("Movement error: %s", e)
            self.error_count += 1
            return {
                "success": False,
                "error": str(e),
                "current_position": self._position_to_dict(self.current_position),
                "target_position": self._position_to_dict(target_position)
            }
    
    async def emergency_stop(self) -> Dict[str, Any]:
        """
        Emergency stop with integrated safety systems
        """
        start_time = datetime.now(timezone.utc)
        
        try:
            logger.critical("Emergency stop initiated")
            
            # Update state
            self.current_state = RobotState.EMERGENCY_STOP
            self.movement_mode = MovementMode.MANUAL
            
            # Execute emergency stop on all systems
            speed_result = await speed_controller.emergency_stop()
            safety_result = await self.safety_service.trigger_emergency_stop()
            await safety_speed_control._trigger_emergency_stop()
            
            # Stop movement (preserve emergency state)
            await self._stop_movement(preserve_state=True)
            
            # Calculate response time
            response_time = (datetime.now(timezone.utc) - start_time).total_seconds() * 1000
            
            success = speed_result.get("success", False) and safety_result.get("success", False)
            
            return {
                "success": success,
                "current_position": self._position_to_dict(self.current_position),
                "state": self.current_state.value,
                "response_time_ms": response_time,
                "speed_control_success": speed_result.get("success", False),
                "safety_system_success": safety_result.get("success", False),
                "timestamp": start_time.isoformat()
            }
            
        except Exception as e:
            logger.error("Emergency stop error: %s", e)
            return {
                "success": False,
                "error": str(e),
                "current_position": self._position_to_dict(self.current_position)
            }
    
    async def set_movement_mode(self, mode: MovementMode) -> Dict[str, Any]:
        """
        Set robot movement mode
        """
        try:
            self.movement_mode = mode
            
            # Update speed controller mode based on movement mode
            if mode == MovementMode.MANUAL:
                speed_mode = SpeedMode.MANUAL
            elif mode == MovementMode.AUTO:
                speed_mode = SpeedMode.AUTO
            elif mode == MovementMode.SEMI_AUTO:
                speed_mode = SpeedMode.AUTO
            elif mode == MovementMode.MAINTENANCE:
                speed_mode = SpeedMode.SAFETY
            else:
                speed_mode = SpeedMode.MANUAL
            
            # Apply speed mode
            await speed_controller.set_speed(
                target_speed=speed_controller.current_speed,
                mode=speed_mode
            )
            
            logger.info("Movement mode changed to: %s", mode.value)
            
            return {
                "success": True,
                "movement_mode": mode.value,
                "speed_mode": speed_mode.value,
                "timestamp": datetime.now(timezone.utc).isoformat()
            }
            
        except Exception as e:
            logger.error("Failed to set movement mode: %s", e)
            return {
                "success": False,
                "error": str(e),
                "current_mode": self.movement_mode.value
            }
    
    async def get_system_status(self) -> Dict[str, Any]:
        """
        Get comprehensive system status
        """
        try:
            # Get status from all subsystems
            speed_status = speed_controller.get_status()
            safety_status = await safety_speed_control.get_safety_status()
            
            # Calculate performance metrics
            avg_response_time = (
                sum(self.response_times) / len(self.response_times) 
                if self.response_times else 0.0
            )
            
            return {
                "success": True,
                "robot_state": self.current_state.value,
                "movement_mode": self.movement_mode.value,
                "current_position": self._position_to_dict(self.current_position),
                "target_position": self._position_to_dict(self.target_position),
                "speed_control": speed_status,
                "safety_status": safety_status,
                "performance": {
                    "avg_response_time_ms": avg_response_time,
                    "operation_count": self.operation_count,
                    "error_count": self.error_count,
                    "error_rate": self.error_count / max(self.operation_count, 1) * 100
                },
                "last_update": self.last_update.isoformat(),
                "timestamp": datetime.now(timezone.utc).isoformat()
            }
            
        except Exception as e:
            logger.error("Failed to get system status: %s", e)
            return {
                "success": False,
                "error": str(e),
                "timestamp": datetime.now(timezone.utc).isoformat()
            }
    
    def _validate_movement_input(self, target_position: Position, 
                                target_speed: float, 
                                movement_mode: MovementMode) -> bool:
        """Validate movement input parameters"""
        if target_speed < 0 or target_speed > 10.0:
            return False
        
        if not isinstance(target_position, Position):
            return False
        
        if not isinstance(movement_mode, MovementMode):
            return False
        
        return True
    
    async def _check_safety_conditions(self) -> bool:
        """Check if safety conditions are met for movement"""
        try:
            # Check safety system status
            safety_status = await safety_speed_control.get_safety_status()
            
            # Check for emergency stop
            if safety_status.get("emergency_stop_active", False):
                logger.warning("Safety check failed: Emergency stop active")
                return False
            
            # Check for obstacles
            closest_obstacle = safety_status.get("closest_obstacle")
            if closest_obstacle and closest_obstacle.get("distance", float('inf')) < 1.0:
                logger.warning("Safety check failed: Obstacle too close (%.2fm)", closest_obstacle.get("distance", 0))
                return False
            
            return True
            
        except Exception as e:
            logger.error("Safety condition check failed: %s", e)
            return False
    
    async def _execute_movement(self, command: MovementCommand) -> bool:
        """Execute movement command"""
        try:
            # Update target position
            self.target_position = command.target_position
            self.current_state = RobotState.MOVING
            self.last_update = command.timestamp
            
            # Set speed based on movement mode
            if command.movement_mode == MovementMode.MANUAL:
                speed_mode = SpeedMode.MANUAL
            elif command.movement_mode == MovementMode.AUTO:
                speed_mode = SpeedMode.AUTO
            elif command.movement_mode == MovementMode.SEMI_AUTO:
                speed_mode = SpeedMode.AUTO
            elif command.movement_mode == MovementMode.MAINTENANCE:
                speed_mode = SpeedMode.SAFETY
            else:
                speed_mode = SpeedMode.MANUAL
            
            # Execute speed command
            speed_result = await speed_controller.set_speed(
                target_speed=command.target_speed,
                mode=speed_mode,
                safety_override=command.safety_override
            )
            
            if not speed_result.get("success", False):
                logger.error("Speed control failed: %s", speed_result.get("error"))
                return False
            
            # Simulate position update (in real implementation, this would come from position sensors)
            await self._update_position(command.target_position)
            
            # Add to command history
            self.command_history.append(command)
            if len(self.command_history) > 100:  # Keep last 100 commands
                self.command_history.pop(0)
            
            logger.debug("Movement executed: position=(%.2f, %.2f), speed=%.2f, mode=%s",
                        command.target_position.x, command.target_position.y,
                        command.target_speed, command.movement_mode.value)
            
            return True
            
        except Exception as e:
            logger.error("Movement execution failed: %s", e)
            self.current_state = RobotState.ERROR
            return False
    
    async def _update_position(self, new_position: Position):
        """Update robot position (simulated)"""
        # In real implementation, this would read from position sensors
        self.current_position = new_position
        
        # Check if target reached
        distance = ((new_position.x - self.target_position.x) ** 2 + 
                   (new_position.y - self.target_position.y) ** 2) ** 0.5
        
        if distance < 0.1:  # 10cm tolerance
            self.current_state = RobotState.IDLE
            logger.info("Target position reached: (%.2f, %.2f)", new_position.x, new_position.y)
    
    async def _stop_movement(self, preserve_state: bool = False):
        """Stop robot movement"""
        try:
            # Stop speed controller (preserve emergency mode if in emergency state)
            if preserve_state and self.current_state == RobotState.EMERGENCY_STOP:
                await speed_controller.set_speed(0.0, SpeedMode.EMERGENCY)
            else:
                await speed_controller.set_speed(0.0, SpeedMode.MANUAL)
            
            # Update state (preserve emergency state if requested)
            if not preserve_state or self.current_state != RobotState.EMERGENCY_STOP:
                self.current_state = RobotState.IDLE
            self.last_update = datetime.now(timezone.utc)
            
            logger.info("Movement stopped")
            
        except Exception as e:
            logger.error("Failed to stop movement: %s", e)
    
    def _position_to_dict(self, position: Position) -> Dict[str, float]:
        """Convert Position to dictionary"""
        return {
            "x": position.x,
            "y": position.y,
            "theta": position.theta
        }
    
    async def reset_system(self) -> Dict[str, Any]:
        """Reset robot control system"""
        try:
            # Reset state
            self.current_state = RobotState.IDLE
            self.movement_mode = MovementMode.MANUAL
            self.error_count = 0
            self.operation_count = 0
            self.response_times.clear()
            self.command_history.clear()
            
            # Reset position
            self.current_position = Position(0.0, 0.0, 0.0)
            self.target_position = Position(0.0, 0.0, 0.0)
            
            # Reset subsystems
            await speed_controller.set_speed(0.0, SpeedMode.MANUAL)
            await safety_speed_control.reset_emergency_stop()
            
            self.last_update = datetime.now(timezone.utc)
            
            logger.info("Robot control system reset")
            
            return {
                "success": True,
                "message": "System reset successfully",
                "timestamp": self.last_update.isoformat()
            }
            
        except Exception as e:
            logger.error("System reset failed: %s", e)
            return {
                "success": False,
                "error": str(e),
                "timestamp": datetime.now(timezone.utc).isoformat()
            }


# Global robot control system instance
robot_control_system = RobotControlSystem()
