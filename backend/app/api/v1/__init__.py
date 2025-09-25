"""
API v1 Router Aggregation

This module aggregates all v1 API routers into a single router.
"""

from fastapi import APIRouter

from app.api.v1 import auth, robot, telemetry, safety, config, monitoring, fw_integration, module_telemetry

# Create main v1 router
router = APIRouter(prefix="/api/v1")

# Include all v1 routers
router.include_router(auth.router, tags=["authentication"])
router.include_router(robot.router, tags=["robot"])
router.include_router(telemetry.router, tags=["telemetry"])
router.include_router(safety.router, tags=["safety"])
router.include_router(config.router, tags=["configuration"])
router.include_router(monitoring.router, tags=["monitoring"])
router.include_router(fw_integration.router, tags=["firmware-integration"])
router.include_router(module_telemetry.router, tags=["module-telemetry"])

@router.get("/")
async def v1_root():
    """V1 API root endpoint"""
    return {"message": "OHT-50 API v1", "status": "active"}
