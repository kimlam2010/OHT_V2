"""
Robot control API endpoints
"""

from fastapi import APIRouter

router = APIRouter(prefix="/robot", tags=["robot"])


@router.get("/status")
async def get_robot_status():
    """Get robot status"""
    return {"status": "idle", "message": "Robot status endpoint"}


@router.post("/control")
async def control_robot():
    """Control robot"""
    return {"message": "Robot control endpoint"}
