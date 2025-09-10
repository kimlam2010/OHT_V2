"""
Robot control schemas for OHT-50 Backend
"""

from pydantic import BaseModel, Field, field_validator, ValidationInfo
from typing import Dict, Any, Optional, List
from datetime import datetime
from enum import Enum

from .dashboard import RobotMode, RobotStatus


class MovementDirection(str, Enum):
    FORWARD = "forward"
    BACKWARD = "backward"
    LEFT = "left"
    RIGHT = "right"
    STOP = "stop"


class SpeedPreset(str, Enum):
    SLOW = "slow"
    NORMAL = "normal"
    FAST = "fast"
    MAX = "max"


class RobotModeRequest(BaseModel):
    mode: RobotMode = Field(..., description="Target robot mode")
    reason: Optional[str] = Field(None, description="Reason for mode change")
    force: bool = Field(default=False, description="Force mode change even if unsafe")


class RobotModeResponse(BaseModel):
    success: bool = Field(..., description="Mode change success status")
    message: str = Field(..., description="Response message")
    previous_mode: RobotMode = Field(..., description="Previous robot mode")
    current_mode: RobotMode = Field(..., description="Current robot mode")
    timestamp: datetime = Field(..., description="Mode change timestamp")


class MovementRequest(BaseModel):
    direction: MovementDirection = Field(..., description="Movement direction")
    speed: float = Field(..., ge=0.0, le=1.0, description="Movement speed (0.0-1.0)")
    duration: Optional[float] = Field(None, ge=0.0, description="Movement duration in seconds")
    distance: Optional[float] = Field(None, ge=0.0, description="Movement distance in meters")
    
    @field_validator('speed')
    @classmethod
    def validate_speed(cls, v: float) -> float:
        if v < 0.0 or v > 1.0:
            raise ValueError('Speed must be between 0.0 and 1.0')
        return v


class MovementResponse(BaseModel):
    success: bool = Field(..., description="Movement command success status")
    message: str = Field(..., description="Response message")
    direction: MovementDirection = Field(..., description="Movement direction")
    speed: float = Field(..., description="Movement speed")
    estimated_duration: Optional[float] = Field(None, description="Estimated duration in seconds")
    timestamp: datetime = Field(..., description="Command timestamp")


class SpeedRequest(BaseModel):
    speed: float = Field(..., ge=0.0, le=1.0, description="Target speed (0.0-1.0)")
    acceleration: Optional[float] = Field(None, ge=0.0, le=1.0, description="Acceleration rate (0.0-1.0)")
    deceleration: Optional[float] = Field(None, ge=0.0, le=1.0, description="Deceleration rate (0.0-1.0)")
    
    @field_validator('speed')
    @classmethod
    def validate_speed(cls, v: float) -> float:
        if v < 0.0 or v > 1.0:
            raise ValueError('Speed must be between 0.0 and 1.0')
        return v
    
    @field_validator('acceleration')
    @classmethod
    def validate_acceleration(cls, v: Optional[float]) -> Optional[float]:
        if v is not None and (v < 0.0 or v > 1.0):
            raise ValueError('Acceleration must be between 0.0 and 1.0')
        return v
    
    @field_validator('deceleration')
    @classmethod
    def validate_deceleration(cls, v: Optional[float]) -> Optional[float]:
        if v is not None and (v < 0.0 or v > 1.0):
            raise ValueError('Deceleration must be between 0.0 and 1.0')
        return v


class SpeedResponse(BaseModel):
    success: bool = Field(..., description="Speed change success status")
    message: str = Field(..., description="Response message")
    previous_speed: float = Field(..., description="Previous speed")
    current_speed: float = Field(..., description="Current speed")
    acceleration: Optional[float] = Field(None, description="Acceleration rate")
    deceleration: Optional[float] = Field(None, description="Deceleration rate")
    timestamp: datetime = Field(..., description="Speed change timestamp")


class SpeedPresetRequest(BaseModel):
    preset: SpeedPreset = Field(..., description="Speed preset to apply")


class SpeedPresetResponse(BaseModel):
    success: bool = Field(..., description="Preset application success status")
    message: str = Field(..., description="Response message")
    preset: SpeedPreset = Field(..., description="Applied preset")
    speed_value: float = Field(..., description="Actual speed value")
    timestamp: datetime = Field(..., description="Preset application timestamp")


class EmergencyStopResponse(BaseModel):
    success: bool = Field(..., description="Emergency stop success status")
    message: str = Field(..., description="Response message")
    status: RobotStatus = Field(..., description="Robot status after emergency stop")
    timestamp: datetime = Field(..., description="Emergency stop timestamp")
    response_time_ms: float = Field(..., description="Response time in milliseconds")


class PauseResponse(BaseModel):
    success: bool = Field(..., description="Pause success status")
    message: str = Field(..., description="Response message")
    status: RobotStatus = Field(..., description="Robot status after pause")
    timestamp: datetime = Field(..., description="Pause timestamp")


class ResetResponse(BaseModel):
    success: bool = Field(..., description="Reset success status")
    message: str = Field(..., description="Response message")
    status: RobotStatus = Field(..., description="Robot status after reset")
    timestamp: datetime = Field(..., description="Reset timestamp")


class RobotCommand(BaseModel):
    command_type: str = Field(..., description="Command type")
    parameters: Dict[str, Any] = Field(default_factory=dict, description="Command parameters")
    priority: int = Field(default=1, ge=1, le=10, description="Command priority (1-10)")
    timeout: Optional[float] = Field(None, ge=0.0, description="Command timeout in seconds")
    
    @field_validator('command_type')
    @classmethod
    def validate_command_type(cls, v: str) -> str:
        valid_commands = ["move", "stop", "pause", "resume", "reset", "home", "dock", "undock"]
        if v not in valid_commands:
            raise ValueError(f'Command type must be one of: {", ".join(valid_commands)}')
        return v
    
    @field_validator('priority')
    @classmethod
    def validate_priority(cls, v: int) -> int:
        if v < 1 or v > 10:
            raise ValueError('Priority must be between 1 and 10')
        return v


class RobotCommandResponse(BaseModel):
    success: bool = Field(..., description="Command execution success status")
    message: str = Field(..., description="Response message")
    command_id: str = Field(..., description="Command ID")
    command_type: str = Field(..., description="Command type")
    execution_time_ms: float = Field(..., description="Command execution time in milliseconds")
    timestamp: datetime = Field(..., description="Command timestamp")
    result: Optional[Dict[str, Any]] = Field(None, description="Command result data")


class RobotStatusResponse(BaseModel):
    robot_id: str = Field(..., description="Robot ID")
    status: RobotStatus = Field(..., description="Current robot status")
    mode: RobotMode = Field(..., description="Current robot mode")
    position: Dict[str, float] = Field(..., description="Current position")
    battery_level: int = Field(..., description="Battery level percentage")
    temperature: float = Field(..., description="Robot temperature")
    speed: float = Field(..., description="Current speed")
    last_command: Optional[str] = Field(None, description="Last executed command")
    uptime: int = Field(..., description="Robot uptime in seconds")
    health_score: float = Field(..., description="Robot health score (0-100)")
    timestamp: datetime = Field(..., description="Status timestamp")
    created_at: datetime = Field(..., description="Record creation time")
    updated_at: datetime = Field(..., description="Record update time")


class RobotConfiguration(BaseModel):
    max_speed: float = Field(..., description="Maximum robot speed")
    max_acceleration: float = Field(..., description="Maximum acceleration")
    max_deceleration: float = Field(..., description="Maximum deceleration")
    safety_distance: float = Field(..., description="Safety distance in meters")
    emergency_stop_timeout: float = Field(..., description="Emergency stop timeout in seconds")
    battery_warning_threshold: int = Field(..., description="Battery warning threshold percentage")
    temperature_warning_threshold: float = Field(..., description="Temperature warning threshold")
    auto_dock_enabled: bool = Field(..., description="Auto dock feature enabled")
    obstacle_detection_enabled: bool = Field(..., description="Obstacle detection enabled")
    logging_level: str = Field(..., description="Logging level")


class RobotConfigurationResponse(BaseModel):
    success: bool = Field(..., description="Configuration retrieval success status")
    configuration: RobotConfiguration = Field(..., description="Robot configuration")
    last_updated: datetime = Field(..., description="Last configuration update time")


class RobotPosition(BaseModel):
    x: float = Field(..., description="X coordinate")
    y: float = Field(..., description="Y coordinate")
    z: float = Field(..., description="Z coordinate")
    orientation: float = Field(..., description="Orientation angle in degrees")
    accuracy: float = Field(..., description="Position accuracy in meters")
    timestamp: datetime = Field(..., description="Position timestamp")


class RobotPositionResponse(BaseModel):
    success: bool = Field(..., description="Position retrieval success status")
    position: RobotPosition = Field(..., description="Current robot position")
    last_updated: datetime = Field(..., description="Last position update time")


class CommandHistory(BaseModel):
    command_id: str = Field(..., description="Command ID")
    command_type: str = Field(..., description="Command type")
    parameters: Dict[str, Any] = Field(..., description="Command parameters")
    status: str = Field(..., description="Command status")
    executed_at: datetime = Field(..., description="Command execution time")
    completed_at: Optional[datetime] = Field(None, description="Command completion time")
    execution_time_ms: Optional[float] = Field(None, description="Command execution time in milliseconds")
    result: Optional[Dict[str, Any]] = Field(None, description="Command result")


class CommandHistoryResponse(BaseModel):
    commands: List[CommandHistory] = Field(..., description="List of commands")
    total_count: int = Field(..., description="Total number of commands")
    page: int = Field(..., description="Current page number")
    page_size: int = Field(..., description="Number of items per page")
    has_next: bool = Field(..., description="Whether there are more pages")
