"""
Mission API endpoints
"""

from fastapi import APIRouter, Depends, HTTPException, status
from typing import List, Optional
from uuid import UUID

from app.models.mission import (
    Mission, MissionCreate, MissionUpdate, MissionStatus, ValidationResult, 
    ExecutionStatus, MissionQueue
)
from app.services.mission_service import MissionService
from app.services.log_service import LogService

router = APIRouter(tags=["Mission Management"])


def get_mission_service() -> MissionService:
    """Dependency injection cho MissionService"""
    log_service = LogService()
    return MissionService(log_service)


@router.post("/", response_model=Mission, status_code=status.HTTP_201_CREATED)
async def create_mission(
    mission_data: MissionCreate,
    mission_service: MissionService = Depends(get_mission_service)
):
    """Tạo mission mới"""
    try:
        mission = await mission_service.create_mission(mission_data.dict())
        return mission
    except Exception as e:
        raise HTTPException(
            status_code=status.HTTP_400_BAD_REQUEST,
            detail=f"Error creating mission: {str(e)}"
        )


@router.get("/{mission_id}", response_model=Mission)
async def get_mission(
    mission_id: str,
    mission_service: MissionService = Depends(get_mission_service)
):
    """Lấy thông tin mission"""
    try:
        # Find mission in different collections
        mission_uuid = UUID(mission_id)
        
        # Check active missions
        active_missions = await mission_service.get_active_missions()
        for mission in active_missions:
            if mission.id == mission_uuid:
                return mission
        
        # Check completed missions
        completed_missions = await mission_service.get_completed_missions()
        for mission in completed_missions:
            if mission.id == mission_uuid:
                return mission
        
        # Check failed missions
        failed_missions = await mission_service.get_failed_missions()
        for mission in failed_missions:
            if mission.id == mission_uuid:
                return mission
        
        # Check queue
        queue_missions = await mission_service.get_mission_queue()
        for mission in queue_missions:
            if mission.id == mission_uuid:
                return mission
        
        raise HTTPException(
            status_code=status.HTTP_404_NOT_FOUND,
            detail="Mission not found"
        )
        
    except ValueError:
        raise HTTPException(
            status_code=status.HTTP_400_BAD_REQUEST,
            detail="Invalid mission ID format"
        )


@router.put("/{mission_id}", response_model=Mission)
async def update_mission(
    mission_id: str,
    mission_update: MissionUpdate,
    mission_service: MissionService = Depends(get_mission_service)
):
    """Cập nhật mission"""
    try:
        mission = await mission_service.update_mission(mission_id, mission_update.dict(exclude_unset=True))
        if not mission:
            raise HTTPException(
                status_code=status.HTTP_404_NOT_FOUND,
                detail="Mission not found"
            )
        return mission
    except Exception as e:
        raise HTTPException(
            status_code=status.HTTP_400_BAD_REQUEST,
            detail=f"Error updating mission: {str(e)}"
        )


@router.post("/{mission_id}/execute", response_model=ExecutionStatus)
async def execute_mission(
    mission_id: str,
    mission_service: MissionService = Depends(get_mission_service)
):
    """Thực thi mission"""
    try:
        execution_status = await mission_service.execute_mission(mission_id)
        return execution_status
    except Exception as e:
        raise HTTPException(
            status_code=status.HTTP_400_BAD_REQUEST,
            detail=f"Error executing mission: {str(e)}"
        )


@router.get("/{mission_id}/status", response_model=MissionStatus)
async def get_mission_status(
    mission_id: str,
    mission_service: MissionService = Depends(get_mission_service)
):
    """Lấy trạng thái mission"""
    try:
        status = await mission_service.get_mission_status(mission_id)
        return status
    except Exception as e:
        raise HTTPException(
            status_code=status.HTTP_400_BAD_REQUEST,
            detail=f"Error getting mission status: {str(e)}"
        )


@router.delete("/{mission_id}", status_code=status.HTTP_204_NO_CONTENT)
async def cancel_mission(
    mission_id: str,
    mission_service: MissionService = Depends(get_mission_service)
):
    """Hủy mission"""
    try:
        success = await mission_service.cancel_mission(mission_id)
        if not success:
            raise HTTPException(
                status_code=status.HTTP_404_NOT_FOUND,
                detail="Mission not found or cannot be cancelled"
            )
    except Exception as e:
        raise HTTPException(
            status_code=status.HTTP_400_BAD_REQUEST,
            detail=f"Error cancelling mission: {str(e)}"
        )


@router.get("/queue/list", response_model=List[Mission])
async def get_mission_queue(
    mission_service: MissionService = Depends(get_mission_service)
):
    """Lấy danh sách mission trong queue"""
    try:
        missions = await mission_service.get_mission_queue()
        return missions
    except Exception as e:
        raise HTTPException(
            status_code=status.HTTP_400_BAD_REQUEST,
            detail=f"Error getting mission queue: {str(e)}"
        )


@router.get("/active/list", response_model=List[Mission])
async def get_active_missions(
    mission_service: MissionService = Depends(get_mission_service)
):
    """Lấy danh sách mission đang thực thi"""
    try:
        missions = await mission_service.get_active_missions()
        return missions
    except Exception as e:
        raise HTTPException(
            status_code=status.HTTP_400_BAD_REQUEST,
            detail=f"Error getting active missions: {str(e)}"
        )


@router.get("/completed/list", response_model=List[Mission])
async def get_completed_missions(
    mission_service: MissionService = Depends(get_mission_service)
):
    """Lấy danh sách mission đã hoàn thành"""
    try:
        missions = await mission_service.get_completed_missions()
        return missions
    except Exception as e:
        raise HTTPException(
            status_code=status.HTTP_400_BAD_REQUEST,
            detail=f"Error getting completed missions: {str(e)}"
        )


@router.get("/failed/list", response_model=List[Mission])
async def get_failed_missions(
    mission_service: MissionService = Depends(get_mission_service)
):
    """Lấy danh sách mission thất bại"""
    try:
        missions = await mission_service.get_failed_missions()
        return missions
    except Exception as e:
        raise HTTPException(
            status_code=status.HTTP_400_BAD_REQUEST,
            detail=f"Error getting failed missions: {str(e)}"
        )


@router.post("/{mission_id}/validate", response_model=ValidationResult)
async def validate_mission(
    mission_id: str,
    mission_service: MissionService = Depends(get_mission_service)
):
    """Validate mission"""
    try:
        # Get mission first
        mission_uuid = UUID(mission_id)
        
        # Find mission in queue
        queue_missions = await mission_service.get_mission_queue()
        mission = None
        for m in queue_missions:
            if m.id == mission_uuid:
                mission = m
                break
        
        if not mission:
            raise HTTPException(
                status_code=status.HTTP_404_NOT_FOUND,
                detail="Mission not found"
            )
        
        validation_result = await mission_service.validate_mission(mission)
        return validation_result
        
    except ValueError:
        raise HTTPException(
            status_code=status.HTTP_400_BAD_REQUEST,
            detail="Invalid mission ID format"
        )
    except Exception as e:
        raise HTTPException(
            status_code=status.HTTP_400_BAD_REQUEST,
            detail=f"Error validating mission: {str(e)}"
        )
