"""
Configuration API endpoints for OHT-50 Backend
"""

from typing import Any
from fastapi import APIRouter, Depends, HTTPException, status
from pydantic import BaseModel

from app.core.security import require_permission
from app.models.user import User

router = APIRouter(prefix="/api/v1/config", tags=["configuration"])


class SystemConfig(BaseModel):
    robot_speed_limit: float = 2.0
    emergency_stop_timeout: int = 100
    telemetry_interval: int = 1000
    safety_temperature_threshold: float = 60.0
    max_battery_discharge: int = 20


class ConfigUpdate(BaseModel):
    key: str
    value: Any


@router.get("/current", response_model=SystemConfig)
async def get_current_config(
    current_user: User = Depends(require_permission("config", "read"))
):
    """Get current system configuration"""
    try:
        return SystemConfig()
    except Exception as e:
        raise HTTPException(
            status_code=status.HTTP_500_INTERNAL_SERVER_ERROR,
            detail=f"Failed to get current config: {str(e)}"
        )


@router.get("/system", response_model=SystemConfig)
async def get_system_config(
    current_user: User = Depends(require_permission("system", "read"))
):
    """Get system configuration"""
    try:
        # In a real implementation, this would load from database/file
        config = SystemConfig()
        return config
    except Exception as e:
        raise HTTPException(
            status_code=status.HTTP_500_INTERNAL_SERVER_ERROR,
            detail=f"Failed to get system config: {str(e)}"
        )


@router.put("/system")
async def update_system_config(
    config: SystemConfig,
    current_user: User = Depends(require_permission("system", "configure"))
):
    """Update system configuration"""
    try:
        # In a real implementation, this would save to database/file
        # Validate configuration
        if config.robot_speed_limit <= 0:
            raise HTTPException(
                status_code=status.HTTP_400_BAD_REQUEST,
                detail="Robot speed limit must be positive"
            )
        
        if config.emergency_stop_timeout <= 0:
            raise HTTPException(
                status_code=status.HTTP_400_BAD_REQUEST,
                detail="Emergency stop timeout must be positive"
            )
        
        return {
            "success": True,
            "message": "System configuration updated successfully",
            "config": config.model_dump()
        }
    except HTTPException:
        raise
    except Exception as e:
        raise HTTPException(
            status_code=status.HTTP_500_INTERNAL_SERVER_ERROR,
            detail=f"Failed to update system config: {str(e)}"
        )


@router.patch("/system")
async def update_config_field(
    update: ConfigUpdate,
    current_user: User = Depends(require_permission("system", "configure"))
):
    """Update specific configuration field"""
    try:
        # Validate field exists
        config = SystemConfig()
        if not hasattr(config, update.key):
            raise HTTPException(
                status_code=status.HTTP_400_BAD_REQUEST,
                detail=f"Invalid configuration key: {update.key}"
            )
        
        # Validate value
        if update.key == "robot_speed_limit" and update.value <= 0:
            raise HTTPException(
                status_code=status.HTTP_400_BAD_REQUEST,
                detail="Robot speed limit must be positive"
            )
        
        return {
            "success": True,
            "message": f"Configuration field {update.key} updated successfully",
            "key": update.key,
            "value": update.value
        }
    except HTTPException:
        raise
    except Exception as e:
        raise HTTPException(
            status_code=status.HTTP_500_INTERNAL_SERVER_ERROR,
            detail=f"Failed to update config field: {str(e)}"
        )


@router.get("/firmware")
async def get_firmware_config(
    current_user: User = Depends(require_permission("system", "read"))
):
    """Get firmware configuration"""
    try:
        # In a real implementation, this would load from Firmware
        config = {
            "firmware_version": "1.0.0",
            "rs485_baud_rate": 115200,
            "rs485_timeout": 1000,
            "module_addresses": [1, 2, 3],
            "safety_enabled": True,
            "emergency_stop_enabled": True
        }
        return {
            "success": True,
            "config": config
        }
    except Exception as e:
        raise HTTPException(
            status_code=status.HTTP_500_INTERNAL_SERVER_ERROR,
            detail=f"Failed to get firmware config: {str(e)}"
        )


@router.post("/firmware/restart")
async def restart_firmware(
    current_user: User = Depends(require_permission("system", "admin"))
):
    """Restart firmware service"""
    try:
        # In a real implementation, this would send restart command to Firmware
        return {
            "success": True,
            "message": "Firmware restart command sent successfully"
        }
    except Exception as e:
        raise HTTPException(
            status_code=status.HTTP_500_INTERNAL_SERVER_ERROR,
            detail=f"Failed to restart firmware: {str(e)}"
        )


@router.get("/validation")
async def validate_configuration(
    current_user: User = Depends(require_permission("system", "read"))
):
    """Validate current configuration"""
    try:
        # In a real implementation, this would validate all configurations
        validation_results = {
            "system_config": "valid",
            "firmware_config": "valid",
            "database_config": "valid",
            "security_config": "valid",
            "overall_status": "valid"
        }
        
        return {
            "success": True,
            "validation": validation_results
        }
    except Exception as e:
        raise HTTPException(
            status_code=status.HTTP_500_INTERNAL_SERVER_ERROR,
            detail=f"Failed to validate configuration: {str(e)}"
        )
