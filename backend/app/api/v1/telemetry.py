"""
Telemetry API endpoints for OHT-50 Backend
"""

from typing import List, Dict, Any
from fastapi import APIRouter, Depends, HTTPException, status, Query
from pydantic import BaseModel

from app.core.security import require_permission
from app.models.user import User
from app.services.telemetry import telemetry_service

router = APIRouter(prefix="/telemetry", tags=["telemetry"])


class TelemetryData(BaseModel):
    timestamp: str
    motor_speed: float
    motor_temperature: float
    dock_status: str
    safety_status: str


class TelemetrySummary(BaseModel):
    latest_timestamp: str
    total_records: int
    data_points: Dict[str, Any]


@router.get("/current", response_model=TelemetryData)
async def get_current_telemetry(
    current_user: User = Depends(require_permission("telemetry", "read"))
):
    """Get current telemetry data"""
    try:
        telemetry = await telemetry_service.get_current_telemetry()
        
        if "error" in telemetry:
            raise HTTPException(
                status_code=status.HTTP_500_INTERNAL_SERVER_ERROR,
                detail=telemetry["error"]
            )
            
        return TelemetryData(**telemetry)
        
    except HTTPException:
        raise
    except Exception as e:
        raise HTTPException(
            status_code=status.HTTP_500_INTERNAL_SERVER_ERROR,
            detail=f"Failed to get telemetry: {str(e)}"
        )


@router.get("/history", response_model=List[TelemetryData])
async def get_telemetry_history(
    limit: int = Query(default=100, ge=1, le=1000),
    current_user: User = Depends(require_permission("telemetry", "read"))
):
    """Get telemetry history"""
    try:
        history = await telemetry_service.get_telemetry_history(limit)
        return [TelemetryData(**item) for item in history]
        
    except Exception as e:
        raise HTTPException(
            status_code=status.HTTP_500_INTERNAL_SERVER_ERROR,
            detail=f"Failed to get telemetry history: {str(e)}"
        )


@router.get("/summary", response_model=TelemetrySummary)
async def get_telemetry_summary(
    current_user: User = Depends(require_permission("telemetry", "read"))
):
    """Get telemetry summary statistics"""
    try:
        summary = telemetry_service.get_telemetry_summary()
        return TelemetrySummary(**summary)
        
    except Exception as e:
        raise HTTPException(
            status_code=status.HTTP_500_INTERNAL_SERVER_ERROR,
            detail=f"Failed to get telemetry summary: {str(e)}"
        )


@router.get("/modules")
async def get_modules(
    current_user: User = Depends(require_permission("telemetry", "read"))
):
    """Get available modules"""
    try:
        modules = await telemetry_service.discover_modules()
        return {
            "success": True,
            "modules": modules
        }
        
    except Exception as e:
        raise HTTPException(
            status_code=status.HTTP_500_INTERNAL_SERVER_ERROR,
            detail=f"Failed to discover modules: {str(e)}"
        )


@router.get("/modules/{module_id}")
async def get_module_status(
    module_id: str,
    current_user: User = Depends(require_permission("telemetry", "read"))
):
    """Get specific module status"""
    try:
        status = await telemetry_service.get_module_status(module_id)
        
        if "error" in status:
            raise HTTPException(
                status_code=404,
                detail=status["error"]
            )
            
        return {
            "success": True,
            "module_id": module_id,
            "status": status
        }
        
    except HTTPException:
        raise
    except Exception as e:
        raise HTTPException(
            status_code=500,
            detail=f"Failed to get module status: {str(e)}"
        )
