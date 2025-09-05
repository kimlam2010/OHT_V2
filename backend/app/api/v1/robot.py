"""
Robot control API endpoints for OHT-50 Backend
"""

from typing import Dict, Any
from fastapi import APIRouter, Depends, HTTPException, status
from pydantic import BaseModel

from app.core.security import require_permission
from app.models.user import User
from app.services.robot_control import robot_control_service

router = APIRouter(prefix="/robot", tags=["robot"])


class RobotCommand(BaseModel):
    type: str = None
    command_type: str = None
    parameters: Dict[str, Any] = {}
    
    def __init__(self, **data):
        super().__init__(**data)
        # Support both formats: type or command_type
        if not self.type and self.command_type:
            self.type = self.command_type
        elif not self.command_type and self.type:
            self.command_type = self.type
        
        # Ensure at least one is set
        if not self.type:
            raise ValueError("Either 'type' or 'command_type' must be provided")


class RobotStatusResponse(BaseModel):
    robot_id: str
    status: str
    position: Dict[str, float]
    battery_level: int
    temperature: float
    timestamp: str
    created_at: str
    updated_at: str


@router.get("/status", response_model=RobotStatusResponse)
async def get_robot_status(
    current_user: User = Depends(require_permission("robot", "read"))
):
    """Get current robot status"""
    try:
        status_data = await robot_control_service.get_robot_status()
        return RobotStatusResponse(**status_data)
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
                "status": "emergency_stop"
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
