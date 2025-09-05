"""
Speed Control API endpoints for OHT-50
Real-time speed control with safety integration
"""

import logging
from typing import Dict, Any, Optional
from datetime import datetime, timezone

from fastapi import APIRouter, HTTPException, Depends, status
from pydantic import BaseModel, Field, validator

from app.core.security import require_permission
from app.services.speed_control import speed_controller, SpeedMode, SpeedLimits
from app.schemas.user import UserResponse as User

logger = logging.getLogger(__name__)

router = APIRouter()


class SpeedCommandRequest(BaseModel):
    """Speed command request model"""
    target_speed: float = Field(..., ge=0, le=10.0, description="Target speed in m/s")
    mode: str = Field(default="manual", description="Speed control mode")
    safety_override: bool = Field(default=False, description="Override safety limits")
    
    @validator('mode')
    def validate_mode(cls, v: str) -> str:
        valid_modes = [mode.value for mode in SpeedMode]
        if v not in valid_modes:
            raise ValueError(f"Mode must be one of: {valid_modes}")
        return v


class SpeedLimitsRequest(BaseModel):
    """Speed limits configuration request"""
    max_speed: float = Field(..., ge=0.1, le=10.0)
    max_acceleration: float = Field(..., ge=0.1, le=5.0)
    max_deceleration: float = Field(..., ge=0.1, le=10.0)
    safety_speed: float = Field(..., ge=0.1, le=2.0)
    emergency_speed: float = Field(..., ge=0.01, le=1.0)


class SpeedResponse(BaseModel):
    """Speed control response model"""
    success: bool
    current_speed: float
    target_speed: float
    mode: str
    response_time_ms: float
    safety_active: bool
    timestamp: str
    error: Optional[str] = None


@router.post("/set-speed", response_model=SpeedResponse)
async def set_speed(
    request: SpeedCommandRequest,
    current_user: User = Depends(require_permission("robot", "control"))
) -> SpeedResponse:
    """
    Set robot speed with safety validation
    Response time target: < 5ms
    """
    try:
        logger.info("Speed control request from user %s: %.2f m/s, mode: %s", 
                   current_user.username, request.target_speed, request.mode)
        
        # Convert string mode to enum
        mode = SpeedMode(request.mode)
        
        # Execute speed command
        result = await speed_controller.set_speed(
            target_speed=request.target_speed,
            mode=mode,
            safety_override=request.safety_override
        )
        
        # Check response time
        if result.get("response_time_ms", 0) > 5.0:
            logger.warning("Speed control response time exceeded target: %.2f ms", 
                          result.get("response_time_ms", 0))
        
        return SpeedResponse(**result)
        
    except ValueError as e:
        logger.error("Invalid speed control request: %s", e)
        raise HTTPException(
            status_code=status.HTTP_400_BAD_REQUEST,
            detail=str(e)
        )
    except Exception as e:
        logger.error("Speed control error: %s", e)
        raise HTTPException(
            status_code=status.HTTP_500_INTERNAL_SERVER_ERROR,
            detail="Speed control failed"
        )


@router.post("/emergency-stop", response_model=SpeedResponse)
async def emergency_stop(
    current_user: User = Depends(require_permission("safety", "override"))
) -> SpeedResponse:
    """
    Emergency stop - immediate speed reduction to 0
    Response time target: < 5ms
    """
    try:
        logger.warning("Emergency stop requested by user %s", current_user.username)
        
        # Execute emergency stop
        result = await speed_controller.emergency_stop()
        
        # Check response time
        if result.get("response_time_ms", 0) > 5.0:
            logger.warning("Emergency stop response time exceeded target: %.2f ms", 
                          result.get("response_time_ms", 0))
        
        return SpeedResponse(**result)
        
    except Exception as e:
        logger.error("Emergency stop error: %s", e)
        raise HTTPException(
            status_code=status.HTTP_500_INTERNAL_SERVER_ERROR,
            detail="Emergency stop failed"
        )


@router.get("/status")
async def get_speed_status(
    current_user: User = Depends(require_permission("robot", "read"))
) -> Dict[str, Any]:
    """Get current speed controller status"""
    try:
        status_data = speed_controller.get_status()
        return {
            "success": True,
            "data": status_data,
            "timestamp": datetime.now(timezone.utc).isoformat()
        }
    except Exception as e:
        logger.error("Failed to get speed status: %s", e)
        raise HTTPException(
            status_code=status.HTTP_500_INTERNAL_SERVER_ERROR,
            detail="Failed to get speed status"
        )


@router.get("/performance")
async def get_performance_metrics(
    current_user: User = Depends(require_permission("system", "read"))
) -> Dict[str, Any]:
    """Get speed controller performance metrics"""
    try:
        metrics = speed_controller.get_performance_metrics()
        return {
            "success": True,
            "data": metrics,
            "timestamp": datetime.now(timezone.utc).isoformat()
        }
    except Exception as e:
        logger.error("Failed to get performance metrics: %s", e)
        raise HTTPException(
            status_code=status.HTTP_500_INTERNAL_SERVER_ERROR,
            detail="Failed to get performance metrics"
        )


@router.post("/configure-limits")
async def configure_speed_limits(
    request: SpeedLimitsRequest,
    current_user: User = Depends(require_permission("configuration", "write"))
) -> Dict[str, Any]:
    """Configure speed limits"""
    try:
        logger.info("Speed limits configuration by user %s", current_user.username)
        
        # Update speed limits
        new_limits = SpeedLimits(
            max_speed=request.max_speed,
            max_acceleration=request.max_acceleration,
            max_deceleration=request.max_deceleration,
            safety_speed=request.safety_speed,
            emergency_speed=request.emergency_speed
        )
        
        speed_controller.limits = new_limits
        
        return {
            "success": True,
            "message": "Speed limits updated successfully",
            "limits": {
                "max_speed": new_limits.max_speed,
                "max_acceleration": new_limits.max_acceleration,
                "max_deceleration": new_limits.max_deceleration,
                "safety_speed": new_limits.safety_speed,
                "emergency_speed": new_limits.emergency_speed
            },
            "timestamp": datetime.now(timezone.utc).isoformat()
        }
        
    except Exception as e:
        logger.error("Failed to configure speed limits: %s", e)
        raise HTTPException(
            status_code=status.HTTP_500_INTERNAL_SERVER_ERROR,
            detail="Failed to configure speed limits"
        )


@router.post("/safety-status")
async def update_safety_status(
    safety_active: bool,
    obstacle_detected: bool = False,
    current_user: User = Depends(require_permission("safety", "write"))
) -> Dict[str, Any]:
    """Update safety status for speed control"""
    try:
        logger.info("Safety status update by user %s: active=%s, obstacle=%s", 
                   current_user.username, safety_active, obstacle_detected)
        
        # Update safety status
        speed_controller.set_safety_status(safety_active, obstacle_detected)
        
        return {
            "success": True,
            "message": "Safety status updated successfully",
            "safety_active": safety_active,
            "obstacle_detected": obstacle_detected,
            "timestamp": datetime.now(timezone.utc).isoformat()
        }
        
    except Exception as e:
        logger.error("Failed to update safety status: %s", e)
        raise HTTPException(
            status_code=status.HTTP_500_INTERNAL_SERVER_ERROR,
            detail="Failed to update safety status"
        )
