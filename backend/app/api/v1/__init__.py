"""
API v1 Router Aggregation

This module aggregates all v1 API routers into a single router.
"""

from fastapi import APIRouter

from app.api.v1 import auth, robot, telemetry, safety, monitoring
from app.api.v1 import deprecated

# Create main v1 router
router = APIRouter(prefix="/api/v1")

# Include all v1 routers
router.include_router(auth.router, tags=["authentication"])
router.include_router(robot.router, tags=["robot"])
router.include_router(telemetry.router, tags=["telemetry"])
router.include_router(safety.router, tags=["safety"])
router.include_router(monitoring.router, tags=["monitoring"])

# Deprecated groups → 410 Gone with hints
router.include_router(deprecated.router)

@router.get("/")
async def v1_root():
    """V1 API root endpoint"""
    return {"message": "OHT-50 API v1", "status": "active"}
