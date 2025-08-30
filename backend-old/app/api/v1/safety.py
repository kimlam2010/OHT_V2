"""
Safety API endpoints
"""

from fastapi import APIRouter, Depends, HTTPException, status
from typing import List, Optional

from app.models.safety import (
    SafetyZone, CollisionAlert, EmergencyStatus, ZoneStatus, 
    SafetyStatus, SafetyConfig, SafetyEvent
)
from app.services.safety_service import SafetyService
from app.services.log_service import LogService

router = APIRouter(tags=["Safety System"])


def get_safety_service() -> SafetyService:
    """Dependency injection cho SafetyService"""
    log_service = LogService()
    return SafetyService(log_service)


@router.post("/estop", response_model=EmergencyStatus)
async def handle_estop(
    safety_service: SafetyService = Depends(get_safety_service)
):
    """Xử lý E-Stop"""
    try:
        emergency_status = await safety_service.handle_estop()
        return emergency_status
    except Exception as e:
        raise HTTPException(
            status_code=status.HTTP_500_INTERNAL_SERVER_ERROR,
            detail=f"Error handling E-Stop: {str(e)}"
        )


@router.get("/zones/monitor", response_model=List[ZoneStatus])
async def monitor_safety_zones(
    safety_service: SafetyService = Depends(get_safety_service)
):
    """Giám sát vùng an toàn"""
    try:
        zone_statuses = await safety_service.monitor_safety_zones()
        return zone_statuses
    except Exception as e:
        raise HTTPException(
            status_code=status.HTTP_500_INTERNAL_SERVER_ERROR,
            detail=f"Error monitoring safety zones: {str(e)}"
        )


@router.post("/collision/detect", response_model=Optional[CollisionAlert])
async def detect_collision(
    safety_service: SafetyService = Depends(get_safety_service)
):
    """Phát hiện va chạm"""
    try:
        collision_alert = await safety_service.detect_collision()
        return collision_alert
    except Exception as e:
        raise HTTPException(
            status_code=status.HTTP_500_INTERNAL_SERVER_ERROR,
            detail=f"Error detecting collision: {str(e)}"
        )


@router.post("/emergency/execute", response_model=bool)
async def execute_emergency_procedure(
    safety_service: SafetyService = Depends(get_safety_service)
):
    """Thực hiện thủ tục khẩn cấp"""
    try:
        success = await safety_service.execute_emergency_procedure()
        return success
    except Exception as e:
        raise HTTPException(
            status_code=status.HTTP_500_INTERNAL_SERVER_ERROR,
            detail=f"Error executing emergency procedure: {str(e)}"
        )


@router.get("/status", response_model=SafetyStatus)
async def get_safety_status(
    safety_service: SafetyService = Depends(get_safety_service)
):
    """Lấy trạng thái an toàn tổng thể"""
    try:
        safety_status = await safety_service.get_safety_status()
        return safety_status
    except Exception as e:
        raise HTTPException(
            status_code=status.HTTP_500_INTERNAL_SERVER_ERROR,
            detail=f"Error getting safety status: {str(e)}"
        )


@router.post("/zones/add", response_model=SafetyZone)
async def add_safety_zone(
    zone_data: dict,
    safety_service: SafetyService = Depends(get_safety_service)
):
    """Thêm vùng an toàn"""
    try:
        zone = await safety_service.add_safety_zone(zone_data)
        return zone
    except Exception as e:
        raise HTTPException(
            status_code=status.HTTP_400_BAD_REQUEST,
            detail=f"Error adding safety zone: {str(e)}"
        )


@router.delete("/zones/{zone_id}", status_code=status.HTTP_204_NO_CONTENT)
async def remove_safety_zone(
    zone_id: str,
    safety_service: SafetyService = Depends(get_safety_service)
):
    """Xóa vùng an toàn"""
    try:
        success = await safety_service.remove_safety_zone(zone_id)
        if not success:
            raise HTTPException(
                status_code=status.HTTP_404_NOT_FOUND,
                detail="Safety zone not found"
            )
    except Exception as e:
        raise HTTPException(
            status_code=status.HTTP_400_BAD_REQUEST,
            detail=f"Error removing safety zone: {str(e)}"
        )


@router.put("/config", response_model=SafetyConfig)
async def update_safety_config(
    config_data: dict,
    safety_service: SafetyService = Depends(get_safety_service)
):
    """Cập nhật cấu hình an toàn"""
    try:
        config = await safety_service.update_safety_config(config_data)
        return config
    except Exception as e:
        raise HTTPException(
            status_code=status.HTTP_400_BAD_REQUEST,
            detail=f"Error updating safety config: {str(e)}"
        )


@router.get("/config", response_model=SafetyConfig)
async def get_safety_config(
    safety_service: SafetyService = Depends(get_safety_service)
):
    """Lấy cấu hình an toàn"""
    try:
        return safety_service.safety_config
    except Exception as e:
        raise HTTPException(
            status_code=status.HTTP_500_INTERNAL_SERVER_ERROR,
            detail=f"Error getting safety config: {str(e)}"
        )


@router.post("/monitoring/start")
async def start_safety_monitoring(
    safety_service: SafetyService = Depends(get_safety_service)
):
    """Bắt đầu giám sát an toàn"""
    try:
        await safety_service.start_monitoring()
        return {"message": "Safety monitoring started"}
    except Exception as e:
        raise HTTPException(
            status_code=status.HTTP_500_INTERNAL_SERVER_ERROR,
            detail=f"Error starting safety monitoring: {str(e)}"
        )


@router.post("/monitoring/stop")
async def stop_safety_monitoring(
    safety_service: SafetyService = Depends(get_safety_service)
):
    """Dừng giám sát an toàn"""
    try:
        await safety_service.stop_monitoring()
        return {"message": "Safety monitoring stopped"}
    except Exception as e:
        raise HTTPException(
            status_code=status.HTTP_500_INTERNAL_SERVER_ERROR,
            detail=f"Error stopping safety monitoring: {str(e)}"
        )


@router.post("/position/update")
async def update_position(
    position: dict,
    safety_service: SafetyService = Depends(get_safety_service)
):
    """Cập nhật vị trí hiện tại"""
    try:
        from app.models.movement import Position
        pos = Position(**position)
        await safety_service.update_position(pos)
        return {"message": "Position updated"}
    except Exception as e:
        raise HTTPException(
            status_code=status.HTTP_400_BAD_REQUEST,
            detail=f"Error updating position: {str(e)}"
        )


@router.post("/alerts/{alert_id}/resolve", response_model=bool)
async def resolve_alert(
    alert_id: str,
    safety_service: SafetyService = Depends(get_safety_service)
):
    """Giải quyết cảnh báo"""
    try:
        success = await safety_service.resolve_alert(alert_id)
        return success
    except Exception as e:
        raise HTTPException(
            status_code=status.HTTP_400_BAD_REQUEST,
            detail=f"Error resolving alert: {str(e)}"
        )


@router.get("/events", response_model=List[SafetyEvent])
async def get_safety_events(
    hours: int = 24,
    safety_service: SafetyService = Depends(get_safety_service)
):
    """Lấy sự kiện an toàn trong khoảng thời gian"""
    try:
        events = await safety_service.get_safety_events(hours)
        return events
    except Exception as e:
        raise HTTPException(
            status_code=status.HTTP_500_INTERNAL_SERVER_ERROR,
            detail=f"Error getting safety events: {str(e)}"
        )
