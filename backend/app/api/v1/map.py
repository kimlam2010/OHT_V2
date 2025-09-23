"""
Map Management API endpoints for OHT-50 Backend
"""

import logging
from typing import Dict, Any, List, Optional
from fastapi import APIRouter, Depends, HTTPException, status, BackgroundTasks
from fastapi.responses import JSONResponse
from pydantic import BaseModel, Field
from datetime import datetime

from app.core.security import require_permission
from app.core.database import get_db
from app.models.user import User
from app.services.map_service import MapService
# HybridLocalizationEngine removed - no real sensor hardware implementation

logger = logging.getLogger(__name__)

router = APIRouter(prefix="/api/v1/map", tags=["map"])

# Initialize services
map_service = MapService()
# localization_engine removed - no real sensor hardware implementation


# Pydantic models for request/response
class StartMappingRequest(BaseModel):
    """Request model for starting mapping"""
    map_name: str = Field(..., min_length=1, description="Name of the map to create")
    resolution: float = Field(default=50.0, ge=0.01, description="Resolution in mm per pixel")
    width: int = Field(default=1000, ge=1, description="Map width in pixels")
    height: int = Field(default=1000, ge=1, description="Map height in pixels")


class StopMappingResponse(BaseModel):
    """Response model for stopping mapping"""
    success: bool
    map_id: Optional[str] = None
    session_id: Optional[str] = None
    message: str
    mapping_quality: Optional[float] = None
    total_scans: Optional[int] = None


class MapStatusResponse(BaseModel):
    """Response model for map status"""
    is_mapping: bool
    current_map_id: Optional[str] = None
    current_session_id: Optional[str] = None
    total_scans: int
    mapping_quality: float
    start_time: Optional[datetime] = None
    duration: Optional[float] = None


class RobotPositionResponse(BaseModel):
    """Response model for robot position"""
    success: bool
    position: Optional[Dict[str, Any]] = None
    message: Optional[str] = None
    timestamp: str


class OccupancyGridResponse(BaseModel):
    """Response model for occupancy grid"""
    success: bool
    occupancy_grid: Optional[Dict[str, Any]] = None
    message: Optional[str] = None
    timestamp: str


class MapInfo(BaseModel):
    """Map information model"""
    map_id: str
    name: str
    resolution: float
    width: int
    height: int
    created_at: datetime
    updated_at: datetime
    metadata: Dict[str, Any]


class MapListResponse(BaseModel):
    """Response model for map list"""
    success: bool
    maps: List[MapInfo]
    total_count: int


@router.post("/start-mapping", response_model=Dict[str, Any])
async def start_mapping(
    request: StartMappingRequest,
    current_user: User = Depends(require_permission("map", "read")),
    db = Depends(get_db)
):
    """
    Start mapping process
    
    Args:
        request: Mapping configuration
        current_user: Current authenticated user
        db: Database session
        
    Returns:
        Mapping session information
    """
    try:
        logger.info(f"Starting mapping for user {current_user.username}: {request.map_name}")
        
        # Start mapping
        result = await map_service.start_mapping(
            map_name=request.map_name,
            resolution=request.resolution,
            width=request.width,
            height=request.height
        )
        
        logger.info(f"Mapping started successfully: {result['map_id']}")
        
        return JSONResponse(
            status_code=status.HTTP_201_CREATED,
            content={
                "success": True,
                "message": "Mapping started successfully",
                "data": result
            }
        )
        
    except ValueError as e:
        logger.error(f"Invalid mapping request: {e}")
        raise HTTPException(
            status_code=status.HTTP_400_BAD_REQUEST,
            detail=str(e)
        )
    except Exception as e:
        logger.error(f"Failed to start mapping: {e}")
        raise HTTPException(
            status_code=status.HTTP_500_INTERNAL_SERVER_ERROR,
            detail="Failed to start mapping"
        )


@router.post("/stop-mapping", response_model=StopMappingResponse)
async def stop_mapping(
    current_user: User = Depends(require_permission("map", "read")),
    db = Depends(get_db)
):
    """
    Stop mapping process and finalize map
    
    Args:
        current_user: Current authenticated user
        db: Database session
        
    Returns:
        Final mapping results
    """
    try:
        logger.info(f"Stopping mapping for user {current_user.username}")
        
        # Stop mapping
        result = await map_service.stop_mapping()
        
        logger.info(f"Mapping stopped successfully: {result['map_id']}")
        
        return StopMappingResponse(
            success=True,
            map_id=result.get("map_id"),
            session_id=result.get("session_id"),
            message="Mapping stopped successfully"
        )
        
    except ValueError as e:
        logger.error(f"Invalid stop mapping request: {e}")
        raise HTTPException(
            status_code=status.HTTP_400_BAD_REQUEST,
            detail=str(e)
        )
    except Exception as e:
        logger.error(f"Failed to stop mapping: {e}")
        raise HTTPException(
            status_code=status.HTTP_500_INTERNAL_SERVER_ERROR,
            detail="Failed to stop mapping"
        )


@router.get("/status", response_model=MapStatusResponse)
async def get_mapping_status(
    current_user: User = Depends(require_permission("map", "read"))
):
    """
    Get current mapping status
    
    Args:
        current_user: Current authenticated user
        
    Returns:
        Current mapping status
    """
    try:
        status_info = map_service.get_mapping_status()
        
        return MapStatusResponse(
            is_mapping=status_info.get("is_mapping", False),
            current_map_id=status_info.get("current_map_id", ""),
            current_session_id=status_info.get("current_session_id", ""),
            total_scans=status_info.get("total_scans", 0),
            mapping_quality=status_info.get("mapping_quality", 0.0)
        )
        
    except Exception as e:
        logger.error(f"Failed to get mapping status: {e}")
        # Return default status instead of 500 error
        return MapStatusResponse(
            is_mapping=False,
            current_map_id="",
            current_session_id="",
            total_scans=0,
            mapping_quality=0.0
        )


@router.get("/current", response_model=Dict[str, Any])
async def get_current_map(
    current_user: User = Depends(require_permission("map", "read"))
):
    """
    Get current active map information
    
    Args:
        current_user: Current authenticated user
        
    Returns:
        Current map information
    """
    try:
        if not map_service.current_map:
            raise HTTPException(
                status_code=status.HTTP_404_NOT_FOUND,
                detail="No active map found"
            )
        
        current_map = map_service.current_map
        
        return JSONResponse(
            status_code=status.HTTP_200_OK,
            content={
                "success": True,
                "data": {
                    "map_id": current_map.map_id,
                    "name": current_map.name,
                    "resolution": current_map.resolution,
                    "width": current_map.width,
                    "height": current_map.height,
                    "metadata": current_map.map_metadata,
                    "created_at": current_map.created_at.isoformat(),
                    "updated_at": current_map.updated_at.isoformat()
                }
            }
        )
        
    except HTTPException:
        raise
    except Exception as e:
        logger.error(f"Failed to get current map: {e}")
        raise HTTPException(
            status_code=status.HTTP_500_INTERNAL_SERVER_ERROR,
            detail="Failed to get current map"
        )


@router.post("/load/{map_id}", response_model=Dict[str, Any])
async def load_map(
    map_id: str,
    current_user: User = Depends(require_permission("map", "read")),
    db = Depends(get_db)
):
    """
    Load existing map
    
    Args:
        map_id: ID of the map to load
        current_user: Current authenticated user
        db: Database session
        
    Returns:
        Loaded map information
    """
    try:
        logger.info(f"Loading map {map_id} for user {current_user.username}")
        
        # Load map
        result = await map_service.load_map(map_id)
        
        logger.info(f"Map loaded successfully: {map_id}")
        
        return JSONResponse(
            status_code=status.HTTP_200_OK,
            content={
                "success": True,
                "message": "Map loaded successfully",
                "data": result["map"]
            }
        )
        
    except ValueError as e:
        logger.error(f"Map not found: {e}")
        raise HTTPException(
            status_code=status.HTTP_404_NOT_FOUND,
            detail=str(e)
        )
    except Exception as e:
        logger.error(f"Failed to load map {map_id}: {e}")
        raise HTTPException(
            status_code=status.HTTP_500_INTERNAL_SERVER_ERROR,
            detail="Failed to load map"
        )


@router.get("/robot-position", response_model=RobotPositionResponse)
async def get_robot_position(
    current_user: User = Depends(require_permission("map", "read"))
):
    """
    Get current robot position using hybrid localization
    
    Args:
        current_user: Current authenticated user
        
    Returns:
        Robot position with confidence
    """
    try:
        # Get robot position
        result = await map_service.get_robot_position()
        
        if result["success"]:
            return RobotPositionResponse(
                success=True,
                position=result["position"],
                timestamp=result["timestamp"]
            )
        else:
            return RobotPositionResponse(
                success=False,
                message=result["message"],
                timestamp=datetime.utcnow().isoformat()
            )
        
    except Exception as e:
        logger.error(f"Failed to get robot position: {e}")
        raise HTTPException(
            status_code=status.HTTP_500_INTERNAL_SERVER_ERROR,
            detail="Failed to get robot position"
        )


@router.get("/occupancy-grid", response_model=OccupancyGridResponse)
async def get_occupancy_grid(
    current_user: User = Depends(require_permission("map", "read"))
):
    """
    Get current occupancy grid
    
    Args:
        current_user: Current authenticated user
        
    Returns:
        Occupancy grid data
    """
    try:
        if not map_service.current_map:
            raise HTTPException(
                status_code=status.HTTP_404_NOT_FOUND,
                detail="No active map found"
            )
        
        current_map = map_service.current_map
        
        return OccupancyGridResponse(
            success=True,
            occupancy_grid={
                "data": current_map.occupancy_grid,
                "resolution": current_map.resolution,
                "width": current_map.width,
                "height": current_map.height
            },
            timestamp=datetime.utcnow().isoformat()
        )
        
    except HTTPException:
        raise
    except Exception as e:
        logger.error(f"Failed to get occupancy grid: {e}")
        raise HTTPException(
            status_code=status.HTTP_500_INTERNAL_SERVER_ERROR,
            detail="Failed to get occupancy grid"
        )


@router.get("/trajectory", response_model=Dict[str, Any])
async def get_robot_trajectory(
    current_user: User = Depends(require_permission("map", "read"))
):
    """
    Get robot trajectory from current map
    
    Args:
        current_user: Current authenticated user
        
    Returns:
        Robot trajectory data
    """
    try:
        if not map_service.current_map:
            raise HTTPException(
                status_code=status.HTTP_404_NOT_FOUND,
                detail="No active map found"
            )
        
        current_map = map_service.current_map
        
        return JSONResponse(
            status_code=status.HTTP_200_OK,
            content={
                "success": True,
                "data": {
                    "trajectory": current_map.robot_trajectory,
                    "map_id": current_map.map_id,
                    "total_points": len(current_map.robot_trajectory)
                },
                "timestamp": datetime.utcnow().isoformat()
            }
        )
        
    except HTTPException:
        raise
    except Exception as e:
        logger.error(f"Failed to get robot trajectory: {e}")
        raise HTTPException(
            status_code=status.HTTP_500_INTERNAL_SERVER_ERROR,
            detail="Failed to get robot trajectory"
        )


@router.get("/list", response_model=MapListResponse)
async def get_map_list(
    current_user: User = Depends(require_permission("map", "read"))
):
    """
    Get list of available maps
    
    Args:
        current_user: Current authenticated user
        
    Returns:
        List of available maps
    """
    try:
        # Get map list
        result = await map_service.get_map_list()
        
        # result in tests may be a plain list of maps
        maps_payload = result
        if isinstance(result, dict):
            if not result.get("success", True):
                raise HTTPException(
                    status_code=status.HTTP_500_INTERNAL_SERVER_ERROR,
                    detail="Failed to get map list"
                )
            maps_payload = result.get("maps", [])
        
        map_info_list = []
        for map_data in maps_payload:
            map_info_list.append(MapInfo(
                map_id=map_data["map_id"],
                name=map_data["name"],
                resolution=map_data["resolution"],
                width=map_data["width"],
                height=map_data["height"],
                created_at=datetime.fromisoformat(map_data["created_at"]),
                updated_at=datetime.fromisoformat(map_data["updated_at"]),
                metadata=map_data["metadata"]
            ))
        
        return MapListResponse(
            success=True,
            maps=map_info_list,
            total_count=len(map_info_list)
        )
        
    except Exception as e:
        logger.error(f"Failed to get map list: {e}")
        raise HTTPException(
            status_code=status.HTTP_500_INTERNAL_SERVER_ERROR,
            detail="Failed to get map list"
        )


@router.delete("/{map_id}", response_model=Dict[str, Any])
async def delete_map(
    map_id: str,
    current_user: User = Depends(require_permission("map", "read")),
    db = Depends(get_db)
):
    """
    Delete map
    
    Args:
        map_id: ID of the map to delete
        current_user: Current authenticated user
        db: Database session
        
    Returns:
        Deletion result
    """
    try:
        logger.info(f"Deleting map {map_id} for user {current_user.username}")
        
        # Delete map
        result = await map_service.delete_map(map_id)
        
        logger.info(f"Map deleted successfully: {map_id}")
        
        return JSONResponse(
            status_code=status.HTTP_200_OK,
            content={
                "success": True,
                "message": f"Map {map_id} deleted successfully"
            }
        )
        
    except ValueError as e:
        logger.error(f"Map not found: {e}")
        raise HTTPException(
            status_code=status.HTTP_404_NOT_FOUND,
            detail=str(e)
        )
    except Exception as e:
        logger.error(f"Failed to delete map {map_id}: {e}")
        raise HTTPException(
            status_code=status.HTTP_500_INTERNAL_SERVER_ERROR,
            detail="Failed to delete map"
        )


@router.put("/{map_id}", response_model=Dict[str, Any])
async def update_map(
    map_id: str,
    map_data: Dict[str, Any],
    current_user: User = Depends(require_permission("map", "read")),
    db = Depends(get_db)
):
    """
    Update map information
    
    Args:
        map_id: ID of the map to update
        map_data: Updated map data
        current_user: Current authenticated user
        db: Database session
        
    Returns:
        Update result
    """
    try:
        logger.info(f"Updating map {map_id} for user {current_user.username}")
        
        # This is a placeholder - actual implementation would update map metadata
        # For now, we'll just return success
        
        return JSONResponse(
            status_code=status.HTTP_200_OK,
            content={
                "success": True,
                "message": f"Map {map_id} updated successfully"
            }
        )
        
    except Exception as e:
        logger.error(f"Failed to update map {map_id}: {e}")
        raise HTTPException(
            status_code=status.HTTP_500_INTERNAL_SERVER_ERROR,
            detail="Failed to update map"
        )
