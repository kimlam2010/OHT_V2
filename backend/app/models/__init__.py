"""
Pydantic models cho OHT-50 Backend
"""

from .center_comm import *
from .config import *
from .telemetry import *
from .mission import *
from .safety import *
from .movement import *
from .state import *

__all__ = [
    # Telemetry models
    "TelemetryData",
    "TelemetryStatus",
    "TelemetryTarget",
    "TelemetrySafety",
    "TelemetryEncoder",
    # Config models
    "SystemConfig",
    "HardwareConfig",
    "MotorConfig",
    "EncoderConfig",
    "SafetyConfig",
    "TelemetryConfig",
    "CommunicationConfig",
    # Center communication models
    "HeartbeatMessage",
    "TelemetryMessage",
    "MissionMessage",
    "PermissionMessage",
    "EventMessage",
    # Mission models
    "Mission",
    "MissionCreate",
    "MissionUpdate",
    "MissionStatus",
    "MissionPriority",
    "Point",
    "Waypoint",
    "ValidationResult",
    "ExecutionStatus",
    "MissionQueue",
    # Safety models
    "SafetyZone",
    "SafetyZoneType",
    "CollisionAlert",
    "EmergencyStatus",
    "EmergencyLevel",
    "ZoneStatus",
    "SafetyStatus",
    "SafetyConfig",
    "EmergencyProcedure",
    "SafetyEvent",
    # Movement models
    "MovementStatus",
    "TrajectoryType",
    "SpeedProfile",
    "TrajectorySegment",
    "Trajectory",
    "Position",
    "MotorCommand",
    "PIDController",
    "MovementConfig",
    "MovementEvent",
    # State models
    "SystemState",
    "SubsystemState",
    "StateTransition",
    "SubsystemStatus",
    "SystemStatus",
    "StateHistory",
    "StateValidation",
    "StateConfig",
    "StateEvent",
    "StateTransitionRule",
    "StateSnapshot",
]
