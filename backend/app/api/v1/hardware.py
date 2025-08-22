"""
Hardware API endpoints
"""

from fastapi import APIRouter, Depends, HTTPException, status, Query
from typing import List, Optional, Dict, Any

from app.services.hardware_service import HardwareService
from app.services.log_service import LogService

router = APIRouter(tags=["Hardware Control"])


def get_hardware_service() -> HardwareService:
    """Dependency injection cho HardwareService"""
    log_service = LogService()
    return HardwareService(log_service)


@router.get("/status")
async def get_hardware_status(
    hardware_service: HardwareService = Depends(get_hardware_service)
):
    """Lấy trạng thái hardware"""
    try:
        status = await hardware_service.get_hardware_status()
        return {"success": True, "data": status}
    except Exception as e:
        raise HTTPException(
            status_code=status.HTTP_500_INTERNAL_SERVER_ERROR,
            detail=f"Failed to get hardware status: {str(e)}"
        )


@router.get("/health")
async def get_hardware_health(
    hardware_service: HardwareService = Depends(get_hardware_service)
):
    """Lấy health metrics của hardware"""
    try:
        health = await hardware_service.get_hardware_health()
        return {"success": True, "data": health}
    except Exception as e:
        raise HTTPException(
            status_code=status.HTTP_500_INTERNAL_SERVER_ERROR,
            detail=f"Failed to get hardware health: {str(e)}"
        )


# LED Control Endpoints
@router.get("/leds")
async def get_led_status(
    hardware_service: HardwareService = Depends(get_hardware_service)
):
    """Lấy trạng thái tất cả LEDs"""
    try:
        leds = await hardware_service.get_led_status()
        return {"success": True, "data": leds}
    except Exception as e:
        raise HTTPException(
            status_code=status.HTTP_500_INTERNAL_SERVER_ERROR,
            detail=f"Failed to get LED status: {str(e)}"
        )


@router.post("/leds/{led_name}")
async def set_led(
    led_name: str,
    state: bool,
    hardware_service: HardwareService = Depends(get_hardware_service)
):
    """Set trạng thái LED cụ thể"""
    try:
        result = await hardware_service.set_led_state(led_name, state)
        if "error" not in result:
            return {"success": True, "message": f"LED {led_name} set to {state}", "data": result}
        else:
            raise HTTPException(
                status_code=status.HTTP_400_BAD_REQUEST,
                detail=f"Failed to set LED {led_name}: {result.get('error')}"
            )
    except Exception as e:
        raise HTTPException(
            status_code=status.HTTP_500_INTERNAL_SERVER_ERROR,
            detail=f"Failed to set LED: {str(e)}"
        )


@router.post("/leds/all")
async def set_all_leds(
    state: bool,
    hardware_service: HardwareService = Depends(get_hardware_service)
):
    """Set tất cả LEDs cùng lúc"""
    try:
        success = await hardware_service.set_all_leds(state)
        if success:
            return {"success": True, "message": f"All LEDs set to {state}"}
        else:
            raise HTTPException(
                status_code=status.HTTP_400_BAD_REQUEST,
                detail="Failed to set all LEDs"
            )
    except Exception as e:
        raise HTTPException(
            status_code=status.HTTP_500_INTERNAL_SERVER_ERROR,
            detail=f"Failed to set all LEDs: {str(e)}"
        )


# E-Stop Control Endpoints
@router.get("/estop")
async def get_estop_status(
    hardware_service: HardwareService = Depends(get_hardware_service)
):
    """Lấy trạng thái E-Stop"""
    try:
        estop = await hardware_service.get_estop_status()
        return {"success": True, "data": estop}
    except Exception as e:
        raise HTTPException(
            status_code=status.HTTP_500_INTERNAL_SERVER_ERROR,
            detail=f"Failed to get E-Stop status: {str(e)}"
        )


@router.post("/estop/reset")
async def reset_estop(
    hardware_service: HardwareService = Depends(get_hardware_service)
):
    """Reset E-Stop"""
    try:
        success = await hardware_service.reset_estop()
        if success:
            return {"success": True, "message": "E-Stop reset successfully"}
        else:
            raise HTTPException(
                status_code=status.HTTP_400_BAD_REQUEST,
                detail="Failed to reset E-Stop"
            )
    except Exception as e:
        raise HTTPException(
            status_code=status.HTTP_500_INTERNAL_SERVER_ERROR,
            detail=f"Failed to reset E-Stop: {str(e)}"
        )


# Relay Control Endpoints
@router.get("/relays")
async def get_relay_status(
    hardware_service: HardwareService = Depends(get_hardware_service)
):
    """Lấy trạng thái tất cả relays"""
    try:
        relays = await hardware_service.get_relay_status()
        return {"success": True, "data": relays}
    except Exception as e:
        raise HTTPException(
            status_code=status.HTTP_500_INTERNAL_SERVER_ERROR,
            detail=f"Failed to get relay status: {str(e)}"
        )


@router.post("/relays/{relay_name}")
async def set_relay(
    relay_name: str,
    state: bool,
    hardware_service: HardwareService = Depends(get_hardware_service)
):
    """Set trạng thái relay cụ thể"""
    try:
        result = await hardware_service.set_relay_state(relay_name, state)
        if "error" not in result:
            return {"success": True, "message": f"Relay {relay_name} set to {state}", "data": result}
        else:
            raise HTTPException(
                status_code=status.HTTP_400_BAD_REQUEST,
                detail=f"Failed to set relay {relay_name}: {result.get('error')}"
            )
    except Exception as e:
        raise HTTPException(
            status_code=status.HTTP_500_INTERNAL_SERVER_ERROR,
            detail=f"Failed to set relay: {str(e)}"
        )


# Hardware Events Endpoints
@router.get("/events")
async def get_hardware_events(
    limit: int = Query(50, ge=1, le=100, description="Number of events to return"),
    hardware_service: HardwareService = Depends(get_hardware_service)
):
    """Lấy hardware events"""
    try:
        events = await hardware_service.get_hardware_events(limit)
        return {"success": True, "data": events}
    except Exception as e:
        raise HTTPException(
            status_code=status.HTTP_500_INTERNAL_SERVER_ERROR,
            detail=f"Failed to get hardware events: {str(e)}"
        )


# Hardware Control Endpoints
@router.post("/initialize")
async def initialize_hardware(
    hardware_service: HardwareService = Depends(get_hardware_service)
):
    """Khởi tạo hardware service"""
    try:
        success = await hardware_service.initialize()
        if success:
            return {"success": True, "message": "Hardware service initialized"}
        else:
            raise HTTPException(
                status_code=status.HTTP_400_BAD_REQUEST,
                detail="Failed to initialize hardware service"
            )
    except Exception as e:
        raise HTTPException(
            status_code=status.HTTP_500_INTERNAL_SERVER_ERROR,
            detail=f"Failed to initialize hardware: {str(e)}"
        )


@router.post("/shutdown")
async def shutdown_hardware(
    hardware_service: HardwareService = Depends(get_hardware_service)
):
    """Shutdown hardware service"""
    try:
        await hardware_service.shutdown()
        return {"success": True, "message": "Hardware service shutdown"}
    except Exception as e:
        raise HTTPException(
            status_code=status.HTTP_500_INTERNAL_SERVER_ERROR,
            detail=f"Failed to shutdown hardware: {str(e)}"
        )
