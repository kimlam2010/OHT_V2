"""
API v1 package for OHT-50 Backend
"""

from fastapi import APIRouter

router = APIRouter(prefix="/v1", tags=["v1"])

# Import and include all v1 routers here
from .robot import router as robot_router
# from .telemetry import router as telemetry_router
# from .safety import router as safety_router
# from .config import router as config_router
# from .auth import router as auth_router

router.include_router(robot_router)
# router.include_router(telemetry_router)
# router.include_router(safety_router)
# router.include_router(config_router)
# router.include_router(auth_router)

@router.get("/")
async def v1_root():
    """V1 API root endpoint"""
    return {"message": "OHT-50 API v1", "status": "active"}
