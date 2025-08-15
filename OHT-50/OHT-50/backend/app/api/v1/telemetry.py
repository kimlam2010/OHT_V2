"""
Telemetry API endpoints cho OHT-50 Backend
"""
from typing import Any, Dict

from fastapi import APIRouter, BackgroundTasks, WebSocket, WebSocketDisconnect, Query, Depends
import asyncio

from app.services.telemetry_service import telemetry_service
from app.services.log_service import log_service
from app.core.security import verify_token

router = APIRouter()


@router.get("/current", dependencies=[Depends(verify_token)])
async def get_current_telemetry() -> Dict[str, Any]:
    """Get current telemetry data"""
    return {"success": True, "data": telemetry_service.current().model_dump()}


@router.get("/history", dependencies=[Depends(verify_token)])
async def get_telemetry_history(limit: int = Query(100, ge=1, le=1000)) -> Dict[str, Any]:
    """Get telemetry history"""
    return {"success": True, "data": [item.model_dump() for item in telemetry_service.history(limit=limit)]}


@router.get("/logs", dependencies=[Depends(verify_token)])
async def get_logs(limit: int = Query(200, ge=1, le=1000)) -> Dict[str, Any]:
    return {"success": True, "data": log_service.history(limit)}


@router.get("/audit", dependencies=[Depends(verify_token)])
async def get_audit(limit: int = Query(200, ge=1, le=1000), user: str = "", action: str = "") -> Dict[str, Any]:
    return {"success": True, "data": log_service.audit_history(limit, user=user, action=action)}


@router.websocket("/ws")
async def telemetry_ws(ws: WebSocket):
    await ws.accept()
    try:
        while True:
            hz = telemetry_service.get_rate()
            dt = 1.0 / hz
            data = telemetry_service.tick(dt=dt)
            await ws.send_json(data.model_dump())
            await asyncio.sleep(dt)
    except WebSocketDisconnect:
        await ws.close()


@router.post("/rate", dependencies=[Depends(verify_token)])
async def set_rate(hz: float = Query(10.0, ge=0.1, le=50.0)) -> Dict[str, Any]:
    telemetry_service.set_rate(hz)
    return {"success": True, "hz": telemetry_service.get_rate()}


@router.post("/reset", dependencies=[Depends(verify_token)])
async def reset_telemetry() -> Dict[str, Any]:
    telemetry_service.reset()
    return {"success": True}


@router.get("/stats", dependencies=[Depends(verify_token)])
async def get_stats() -> Dict[str, Any]:
    return {"success": True, "data": telemetry_service.stats()}
