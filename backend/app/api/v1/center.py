"""
Center Communication API endpoints cho OHT-50 Backend
"""
from fastapi import APIRouter, Depends
from pydantic import BaseModel, Field
from typing import Any, Dict, Optional
from app.config import get_settings
from app.services.center_client import CenterClient
from app.models.center_comm import Position
from app.core.security import verify_token

router = APIRouter(tags=["Center Integration"])


async def get_center_client():
    settings = get_settings()
    base_url = getattr(settings, 'center_http_base', 'http://localhost:8081')
    return CenterClient(base_url=base_url)


class MissionIn(BaseModel):
    mission_id: str
    mission_type: str
    target_position: Optional[Position] = None
    parameters: Dict[str, Any] = Field(default_factory=dict)
    priority: int = 1
    timeout: Optional[int] = None


_last_mission: Optional[MissionIn] = None


@router.post("/receive-mission", dependencies=[Depends(verify_token)])
async def receive_mission(payload: MissionIn):
    global _last_mission
    _last_mission = payload
    return {"success": True, "accepted": True, "mission_id": payload.mission_id}


class PermissionIn(BaseModel):
    action: str
    resource: Optional[str] = None
    context: Dict[str, Any] = Field(default_factory=dict)


@router.post("/permission", dependencies=[Depends(verify_token)])
async def request_permission(payload: PermissionIn):
    return {"success": True, "allowed": True}


@router.get("/modules", dependencies=[Depends(verify_token)])
async def proxy_modules(client: CenterClient = Depends(get_center_client)):
    data = await client.get_modules()
    return {"success": True, "data": data}


@router.get("/points", dependencies=[Depends(verify_token)])
async def proxy_points(client: CenterClient = Depends(get_center_client)):
    data = await client.get_points()
    return {"success": True, "data": data}
