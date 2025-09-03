"""
Safety API endpoints for OHT-50 Backend
"""

from typing import List, Dict, Any
from fastapi import APIRouter, Depends, HTTPException, status, Query
from pydantic import BaseModel

from app.core.security import require_permission
from app.models.user import User
from app.services.safety import safety_service

router = APIRouter(prefix="/safety", tags=["safety"])


class SafetyStatus(BaseModel):
    status: str
    emergency_stop: bool
    obstacles_detected: bool
    temperature_normal: bool
    timestamp: str


class SafetyAlert(BaseModel):
    id: str
    type: str
    severity: str
    message: str
    timestamp: str
    acknowledged: bool


@router.get("/status", response_model=SafetyStatus)
async def get_safety_status(
    current_user: User = Depends(require_permission("safety", "read"))
):
    """Get current safety status"""
    try:
        safety_data = await safety_service.get_safety_status()
        
        if "error" in safety_data:
            raise HTTPException(
                status_code=status.HTTP_500_INTERNAL_SERVER_ERROR,
                detail=safety_data["error"]
            )
            
        return SafetyStatus(**safety_data)
        
    except HTTPException:
        raise
    except Exception as e:
        raise HTTPException(
            status_code=status.HTTP_500_INTERNAL_SERVER_ERROR,
            detail=f"Failed to get safety status: {str(e)}"
        )


@router.post("/emergency-stop")
async def emergency_stop(
    current_user: User = Depends(require_permission("safety", "acknowledge"))
):
    """Execute emergency stop"""
    try:
        result = await safety_service.emergency_stop()
        
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


@router.get("/alerts", response_model=List[SafetyAlert])
async def get_safety_alerts(
    limit: int = Query(default=50, ge=1, le=100),
    current_user: User = Depends(require_permission("safety", "read"))
):
    """Get safety alerts"""
    try:
        alerts = safety_service.get_safety_alerts(limit)
        return [SafetyAlert(**alert) for alert in alerts]
        
    except Exception as e:
        raise HTTPException(
            status_code=status.HTTP_500_INTERNAL_SERVER_ERROR,
            detail=f"Failed to get safety alerts: {str(e)}"
        )


@router.post("/alerts/{alert_id}/acknowledge")
async def acknowledge_alert(
    alert_id: str,
    current_user: User = Depends(require_permission("safety", "acknowledge"))
):
    """Acknowledge safety alert"""
    try:
        result = await safety_service.acknowledge_alert(alert_id)
        
        if result.get("success"):
            return {
                "success": True,
                "message": "Alert acknowledged successfully",
                "alert": result.get("alert")
            }
        else:
            raise HTTPException(
                status_code=status.HTTP_404_NOT_FOUND,
                detail=result.get("error", "Alert not found")
            )
            
    except HTTPException:
        raise
    except Exception as e:
        raise HTTPException(
            status_code=status.HTTP_500_INTERNAL_SERVER_ERROR,
            detail=f"Failed to acknowledge alert: {str(e)}"
        )
