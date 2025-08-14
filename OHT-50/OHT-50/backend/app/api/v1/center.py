"""
Center Communication API endpoints cho OHT-50 Backend
"""
from fastapi import APIRouter, Depends
from app.config import get_settings
from app.services.center_client import CenterClient

router = APIRouter()


async def get_center_client():
    settings = get_settings()
    base_url = getattr(settings, 'center_http_base', 'http://localhost:8081')
    return CenterClient(base_url=base_url)


@router.post("/mission")
async def receive_mission():
    """Receive mission from Center"""
    return {"message": "Receive mission - TODO"}


@router.post("/permission")
async def request_permission():
    """Request permission from Center"""
    return {"message": "Request permission - TODO"}


@router.get("/modules")
async def proxy_modules(client: CenterClient = Depends(get_center_client)):
    data = await client.get_modules()
    return {"success": True, "data": data}


@router.get("/points")
async def proxy_points(client: CenterClient = Depends(get_center_client)):
    data = await client.get_points()
    return {"success": True, "data": data}
