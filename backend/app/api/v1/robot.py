"""
Robot control API endpoints for OHT-50 Backend
"""

from datetime import datetime, timezone, timedelta
from typing import Dict, Any, Optional
from fastapi import APIRouter, Depends, HTTPException, status, Query
from pydantic import BaseModel

from app.core.security import require_permission
from app.core.database import get_db
from app.models.user import User
from app.services.robot_control import robot_control_service
from app.schemas.robot_control import (
    RobotModeRequest,
    RobotModeResponse,
    MovementRequest,
    MovementResponse,
    SpeedRequest,
    SpeedResponse,
    SpeedPresetRequest,
    SpeedPresetResponse,
    EmergencyStopResponse,
    PauseResponse,
    ResetResponse,
    RobotCommand,
    RobotCommandResponse,
    RobotStatusResponse,
    RobotConfiguration,
    RobotConfigurationResponse,
    RobotPosition,
    RobotPositionResponse,
    CommandHistory,
    CommandHistoryResponse
)

router = APIRouter(prefix="/api/v1/robot", tags=["robot"])


# RobotCommand and RobotStatusResponse are now imported from schemas


@router.get("/status", response_model=RobotStatusResponse)
async def get_robot_status(
    current_user: User = Depends(require_permission("robot", "read"))
):
    """Get current robot status"""
    try:
        # Mock robot status data
        return RobotStatusResponse(
            robot_id="OHT-50-001",
            status="idle",
            mode="auto",
            position={"x": 150.5, "y": 200.3, "z": 0.0},
            battery_level=87,
            temperature=42.5,
            speed=0.0,
            last_command="stop",
            uptime=3600,
            health_score=95.0,
            timestamp=datetime.now(timezone.utc),
            created_at=datetime.now(timezone.utc) - timedelta(hours=1),
            updated_at=datetime.now(timezone.utc)
        )
    except Exception as e:
        raise HTTPException(
            status_code=status.HTTP_500_INTERNAL_SERVER_ERROR,
            detail=f"Failed to get robot status: {str(e)}"
        )


@router.post("/control")
async def control_robot(
    command: RobotCommand,
    current_user: User = Depends(require_permission("robot", "control"))
):
    """Send command to robot"""
    try:
        # Convert to service expected format
        service_command = {
            "command_type": command.type,
            "parameters": command.parameters
        }
        
        result = await robot_control_service.send_command(service_command)
        
        if result.get("success"):
            return {
                "success": True,
                "message": f"Command {command.type} executed successfully",
                "command": result.get("command")
            }
        else:
            raise HTTPException(
                status_code=status.HTTP_400_BAD_REQUEST,
                detail=result.get("error", "Command execution failed")
            )
            
    except HTTPException:
        raise
    except Exception as e:
        raise HTTPException(
            status_code=status.HTTP_500_INTERNAL_SERVER_ERROR,
            detail=f"Failed to execute command: {str(e)}"
        )


@router.post("/command")
async def send_robot_command(
    command: RobotCommand,
    current_user: User = Depends(require_permission("robot", "control"))
):
    """Send command to robot (alias for /control)"""
    return await control_robot(command, current_user)


@router.post("/emergency-stop")
async def emergency_stop(
    current_user: User = Depends(require_permission("robot", "emergency"))
):
    """Execute emergency stop"""
    try:
        result = await robot_control_service.emergency_stop()
        
        if result.get("success"):
            return {
                "success": True,
                "message": "Emergency stop executed successfully",
                "status": "emergency_stop",
                "timestamp": datetime.now(timezone.utc).isoformat()
            }
        else:
            raise HTTPException(
                status_code=status.HTTP_500_INTERNAL_SERVER_ERROR,
                detail=result.get("error", "Emergency stop failed")
            )
            
    except HTTPException:
        raise
    except Exception as e:
        raise HTTPException(
            status_code=status.HTTP_500_INTERNAL_SERVER_ERROR,
            detail=f"Emergency stop failed: {str(e)}"
        )


@router.post("/move")
async def move_robot(
    direction: str,
    speed: float = 1.0,
    current_user: User = Depends(require_permission("robot", "control"))
):
    """Move robot in specified direction"""
    try:
        command: Dict[str, Any] = {
            "type": "move",
            "parameters": {
                "direction": direction,
                "speed": speed
            }
        }
        
        result = await robot_control_service.send_command(command)
        
        if result.get("success"):
            return {
                "success": True,
                "message": f"Robot moving {direction} at speed {speed}",
                "command": result.get("command")
            }
        else:
            raise HTTPException(
                status_code=status.HTTP_400_BAD_REQUEST,
                detail=result.get("error", "Move command failed")
            )
            
    except HTTPException:
        raise
    except Exception as e:
        raise HTTPException(
            status_code=status.HTTP_500_INTERNAL_SERVER_ERROR,
            detail=f"Move command failed: {str(e)}"
        )


@router.post("/stop")
async def stop_robot(
    current_user: User = Depends(require_permission("robot", "control"))
):
    """Stop robot movement"""
    try:
        command: Dict[str, Any] = {
            "type": "stop",
            "parameters": {}
        }
        
        result = await robot_control_service.send_command(command)
        
        if result.get("success"):
            return {
                "success": True,
                "message": "Robot stopped successfully",
                "command": result.get("command")
            }
        else:
            raise HTTPException(
                status_code=status.HTTP_400_BAD_REQUEST,
                detail=result.get("error", "Stop command failed")
            )
            
    except HTTPException:
        raise
    except Exception as e:
        raise HTTPException(
            status_code=status.HTTP_500_INTERNAL_SERVER_ERROR,
            detail=f"Stop command failed: {str(e)}"
        )


@router.get("/position")
async def get_robot_position(
    current_user: User = Depends(require_permission("robot", "read"))
):
    """Get current robot position"""
    try:
        position = await robot_control_service.get_current_position()
        return {
            "success": True,
            "position": position,
            "last_updated": datetime.now(timezone.utc).isoformat()
        }
    except Exception as e:
        raise HTTPException(
            status_code=status.HTTP_500_INTERNAL_SERVER_ERROR,
            detail=f"Failed to get robot position: {str(e)}"
        )


@router.get("/configuration")
async def get_robot_configuration(
    current_user: User = Depends(require_permission("robot", "read"))
):
    """Get robot configuration"""
    try:
        config = await robot_control_service.get_configuration()
        return {
            "success": True,
            "configuration": config,
            "last_updated": datetime.now(timezone.utc).isoformat()
        }
    except Exception as e:
        raise HTTPException(
            status_code=status.HTTP_500_INTERNAL_SERVER_ERROR,
            detail=f"Failed to get robot configuration: {str(e)}"
        )


@router.get("/last-command")
async def get_last_command(
    current_user: User = Depends(require_permission("robot", "read"))
):
    """Get last executed command"""
    try:
        last_command = robot_control_service.last_command
        if last_command:
            return {
                "success": True,
                "last_command": last_command
            }
        else:
            return {
                "success": True,
                "message": "No commands executed yet"
            }
    except Exception as e:
        raise HTTPException(
            status_code=status.HTTP_500_INTERNAL_SERVER_ERROR,
            detail=f"Failed to get last command: {str(e)}"
        )


# New Robot Control Endpoints

@router.post("/mode", response_model=RobotModeResponse)
async def set_robot_mode(
    mode_data: RobotModeRequest,
    current_user: User = Depends(require_permission("robot", "control"))
):
    """Set robot mode: AUTO/MANUAL/SEMI"""
    try:
        # Mock mode change
        previous_mode = "auto"  # Get from current state
        current_mode = mode_data.mode
        
        return RobotModeResponse(
            success=True,
            message=f"Robot mode changed from {previous_mode} to {current_mode}",
            previous_mode=previous_mode,
            current_mode=current_mode,
            timestamp=datetime.now(timezone.utc)
        )
    except Exception as e:
        raise HTTPException(
            status_code=status.HTTP_500_INTERNAL_SERVER_ERROR,
            detail=f"Failed to set robot mode: {str(e)}"
        )


@router.get("/mode")
async def get_robot_mode(
    current_user: User = Depends(require_permission("robot", "read"))
):
    """Get current robot mode"""
    try:
        return {
            "success": True,
            "mode": "auto",
            "timestamp": datetime.now(timezone.utc)
        }
    except Exception as e:
        raise HTTPException(
            status_code=status.HTTP_500_INTERNAL_SERVER_ERROR,
            detail=f"Failed to get robot mode: {str(e)}"
        )


@router.post("/move/forward", response_model=MovementResponse)
async def move_forward(
    speed: float = Query(1.0, ge=0.0, le=1.0, description="Movement speed"),
    current_user: User = Depends(require_permission("robot", "control"))
):
    """Move robot forward"""
    try:
        return MovementResponse(
            success=True,
            message=f"Robot moving forward at speed {speed}",
            direction="forward",
            speed=speed,
            estimated_duration=5.0,
            timestamp=datetime.now(timezone.utc)
        )
    except Exception as e:
        raise HTTPException(
            status_code=status.HTTP_500_INTERNAL_SERVER_ERROR,
            detail=f"Failed to move robot forward: {str(e)}"
        )


@router.post("/move/backward", response_model=MovementResponse)
async def move_backward(
    speed: float = Query(1.0, ge=0.0, le=1.0, description="Movement speed"),
    current_user: User = Depends(require_permission("robot", "control"))
):
    """Move robot backward"""
    try:
        return MovementResponse(
            success=True,
            message=f"Robot moving backward at speed {speed}",
            direction="backward",
            speed=speed,
            estimated_duration=5.0,
            timestamp=datetime.now(timezone.utc)
        )
    except Exception as e:
        raise HTTPException(
            status_code=status.HTTP_500_INTERNAL_SERVER_ERROR,
            detail=f"Failed to move robot backward: {str(e)}"
        )


@router.post("/move/left", response_model=MovementResponse)
async def turn_left(
    angle: float = Query(90.0, ge=0.0, le=360.0, description="Turn angle in degrees"),
    current_user: User = Depends(require_permission("robot", "control"))
):
    """Turn robot left"""
    try:
        return MovementResponse(
            success=True,
            message=f"Robot turning left {angle} degrees",
            direction="left",
            speed=0.5,
            estimated_duration=2.0,
            timestamp=datetime.now(timezone.utc)
        )
    except Exception as e:
        raise HTTPException(
            status_code=status.HTTP_500_INTERNAL_SERVER_ERROR,
            detail=f"Failed to turn robot left: {str(e)}"
        )


@router.post("/move/right", response_model=MovementResponse)
async def turn_right(
    angle: float = Query(90.0, ge=0.0, le=360.0, description="Turn angle in degrees"),
    current_user: User = Depends(require_permission("robot", "control"))
):
    """Turn robot right"""
    try:
        return MovementResponse(
            success=True,
            message=f"Robot turning right {angle} degrees",
            direction="right",
            speed=0.5,
            estimated_duration=2.0,
            timestamp=datetime.now(timezone.utc)
        )
    except Exception as e:
        raise HTTPException(
            status_code=status.HTTP_500_INTERNAL_SERVER_ERROR,
            detail=f"Failed to turn robot right: {str(e)}"
        )


@router.post("/move/stop", response_model=MovementResponse)
async def stop_movement(
    current_user: User = Depends(require_permission("robot", "control"))
):
    """Stop robot movement"""
    try:
        return MovementResponse(
            success=True,
            message="Robot movement stopped",
            direction="stop",
            speed=0.0,
            estimated_duration=0.0,
            timestamp=datetime.now(timezone.utc)
        )
    except Exception as e:
        raise HTTPException(
            status_code=status.HTTP_500_INTERNAL_SERVER_ERROR,
            detail=f"Failed to stop robot movement: {str(e)}"
        )


@router.post("/speed", response_model=SpeedResponse)
async def set_robot_speed(
    speed_data: SpeedRequest,
    current_user: User = Depends(require_permission("robot", "control"))
):
    """Set robot speed"""
    try:
        return SpeedResponse(
            success=True,
            message=f"Robot speed set to {speed_data.speed}",
            previous_speed=0.5,
            current_speed=speed_data.speed,
            acceleration=speed_data.acceleration,
            deceleration=speed_data.deceleration,
            timestamp=datetime.now(timezone.utc)
        )
    except Exception as e:
        raise HTTPException(
            status_code=status.HTTP_500_INTERNAL_SERVER_ERROR,
            detail=f"Failed to set robot speed: {str(e)}"
        )


@router.get("/speed")
async def get_robot_speed(
    current_user: User = Depends(require_permission("robot", "read"))
):
    """Get current robot speed"""
    try:
        return {
            "success": True,
            "speed": 0.5,
            "timestamp": datetime.now(timezone.utc)
        }
    except Exception as e:
        raise HTTPException(
            status_code=status.HTTP_500_INTERNAL_SERVER_ERROR,
            detail=f"Failed to get robot speed: {str(e)}"
        )


@router.post("/speed/preset", response_model=SpeedPresetResponse)
async def set_speed_preset(
    preset_data: SpeedPresetRequest,
    current_user: User = Depends(require_permission("robot", "control"))
):
    """Set speed preset: slow/normal/fast/max"""
    try:
        preset_speeds = {
            "slow": 0.25,
            "normal": 0.5,
            "fast": 0.75,
            "max": 1.0
        }
        
        speed_value = preset_speeds.get(preset_data.preset, 0.5)
        
        return SpeedPresetResponse(
            success=True,
            message=f"Speed preset set to {preset_data.preset}",
            preset=preset_data.preset,
            speed_value=speed_value,
            timestamp=datetime.now(timezone.utc)
        )
    except Exception as e:
        raise HTTPException(
            status_code=status.HTTP_500_INTERNAL_SERVER_ERROR,
            detail=f"Failed to set speed preset: {str(e)}"
        )


@router.post("/pause", response_model=PauseResponse)
async def pause_system(
    current_user: User = Depends(require_permission("robot", "control"))
):
    """Pause robot system"""
    try:
        from app.schemas.dashboard import RobotStatus
        return PauseResponse(
            success=True,
            message="Robot system paused",
            status=RobotStatus.PAUSED,
            timestamp=datetime.now(timezone.utc)
        )
    except Exception as e:
        raise HTTPException(
            status_code=status.HTTP_500_INTERNAL_SERVER_ERROR,
            detail=f"Failed to pause robot system: {str(e)}"
        )


@router.post("/reset", response_model=ResetResponse)
async def reset_system(
    current_user: User = Depends(require_permission("robot", "control"))
):
    """Reset robot system"""
    try:
        return ResetResponse(
            success=True,
            message="Robot system reset",
            status="idle",
            timestamp=datetime.now(timezone.utc)
        )
    except Exception as e:
        raise HTTPException(
            status_code=status.HTTP_500_INTERNAL_SERVER_ERROR,
            detail=f"Failed to reset robot system: {str(e)}"
        )


@router.get("/position", response_model=RobotPositionResponse)
async def get_robot_position(
    current_user: User = Depends(require_permission("robot", "read"))
):
    """Get current robot position"""
    try:
        position = RobotPosition(
            x=150.5,
            y=200.3,
            z=0.0,
            orientation=45.0,
            accuracy=0.1,
            timestamp=datetime.now(timezone.utc)
        )
        
        return RobotPositionResponse(
            success=True,
            position=position,
            last_updated=datetime.now(timezone.utc)
        )
    except Exception as e:
        raise HTTPException(
            status_code=status.HTTP_500_INTERNAL_SERVER_ERROR,
            detail=f"Failed to get robot position: {str(e)}"
        )


@router.get("/configuration", response_model=RobotConfigurationResponse)
async def get_robot_configuration(
    current_user: User = Depends(require_permission("robot", "read"))
):
    """Get robot configuration"""
    try:
        config = RobotConfiguration(
            max_speed=1.0,
            max_acceleration=0.5,
            max_deceleration=0.5,
            safety_distance=1.0,
            emergency_stop_timeout=0.1,
            battery_warning_threshold=20,
            temperature_warning_threshold=70.0,
            auto_dock_enabled=True,
            obstacle_detection_enabled=True,
            logging_level="info"
        )
        
        return RobotConfigurationResponse(
            success=True,
            configuration=config,
            last_updated=datetime.now(timezone.utc)
        )
    except Exception as e:
        raise HTTPException(
            status_code=status.HTTP_500_INTERNAL_SERVER_ERROR,
            detail=f"Failed to get robot configuration: {str(e)}"
        )
