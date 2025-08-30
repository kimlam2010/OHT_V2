"""
Path Planning API endpoints
"""

from fastapi import APIRouter, Depends, HTTPException, status
from typing import List, Optional

from app.models.path_planning import (
    Route, SafeRoute, Obstacle, Point, RouteType, ObstacleType,
    PathPlanningRequest, PathPlanningResult, PathPlanningConfig, OptimizationMetrics,
    PathPlanningEvent
)
from app.services.path_planning_service import PathPlanningService
from app.services.log_service import LogService

router = APIRouter(tags=["Path Planning"])


def get_path_planning_service() -> PathPlanningService:
    """Dependency injection cho PathPlanningService"""
    log_service = LogService()
    return PathPlanningService(log_service)


@router.post("/optimize", response_model=Route)
async def optimize_route(
    waypoints: List[Point],
    path_planning_service: PathPlanningService = Depends(get_path_planning_service)
):
    """Tối ưu hóa route từ danh sách waypoints"""
    try:
        route = await path_planning_service.optimize_route(waypoints)
        return route
    except Exception as e:
        raise HTTPException(
            status_code=status.HTTP_400_BAD_REQUEST,
            detail=f"Error optimizing route: {str(e)}"
        )


@router.post("/avoid-obstacles", response_model=SafeRoute)
async def avoid_obstacles(
    route: Route,
    path_planning_service: PathPlanningService = Depends(get_path_planning_service)
):
    """Tránh chướng ngại vật và tạo route an toàn"""
    try:
        safe_route = await path_planning_service.avoid_obstacles(route)
        return safe_route
    except Exception as e:
        raise HTTPException(
            status_code=status.HTTP_400_BAD_REQUEST,
            detail=f"Error avoiding obstacles: {str(e)}"
        )


@router.post("/integrate-safety", response_model=SafeRoute)
async def integrate_safety_zones(
    route: Route,
    path_planning_service: PathPlanningService = Depends(get_path_planning_service)
):
    """Tích hợp vùng an toàn vào route"""
    try:
        safe_route = await path_planning_service.integrate_safety_zones(route)
        return safe_route
    except Exception as e:
        raise HTTPException(
            status_code=status.HTTP_400_BAD_REQUEST,
            detail=f"Error integrating safety zones: {str(e)}"
        )


@router.post("/generate-waypoints", response_model=List[Point])
async def generate_waypoints(
    mission_data: dict,
    path_planning_service: PathPlanningService = Depends(get_path_planning_service)
):
    """Tạo waypoints từ mission data"""
    try:
        waypoints = await path_planning_service.generate_waypoints(mission_data)
        return waypoints
    except Exception as e:
        raise HTTPException(
            status_code=status.HTTP_400_BAD_REQUEST,
            detail=f"Error generating waypoints: {str(e)}"
        )


@router.post("/plan-complete", response_model=PathPlanningResult)
async def plan_complete_route(
    request: PathPlanningRequest,
    path_planning_service: PathPlanningService = Depends(get_path_planning_service)
):
    """Lập kế hoạch route hoàn chỉnh"""
    try:
        result = await path_planning_service.plan_complete_route(request)
        return result
    except Exception as e:
        raise HTTPException(
            status_code=status.HTTP_400_BAD_REQUEST,
            detail=f"Error planning complete route: {str(e)}"
        )


@router.post("/update-obstacles")
async def update_obstacles(
    obstacles: List[Obstacle],
    path_planning_service: PathPlanningService = Depends(get_path_planning_service)
):
    """Cập nhật danh sách chướng ngại vật"""
    try:
        await path_planning_service.update_obstacles(obstacles)
        return {"message": f"Updated {len(obstacles)} obstacles"}
    except Exception as e:
        raise HTTPException(
            status_code=status.HTTP_400_BAD_REQUEST,
            detail=f"Error updating obstacles: {str(e)}"
        )


@router.post("/update-safety-zones")
async def update_safety_zones(
    safety_zones: List[dict],
    path_planning_service: PathPlanningService = Depends(get_path_planning_service)
):
    """Cập nhật vùng an toàn"""
    try:
        await path_planning_service.update_safety_zones(safety_zones)
        return {"message": f"Updated {len(safety_zones)} safety zones"}
    except Exception as e:
        raise HTTPException(
            status_code=status.HTTP_400_BAD_REQUEST,
            detail=f"Error updating safety zones: {str(e)}"
        )


@router.get("/history", response_model=List[PathPlanningEvent])
async def get_planning_history(
    limit: int = 100,
    path_planning_service: PathPlanningService = Depends(get_path_planning_service)
):
    """Lấy lịch sử lập kế hoạch"""
    try:
        history = await path_planning_service.get_planning_history(limit)
        return history
    except Exception as e:
        raise HTTPException(
            status_code=status.HTTP_400_BAD_REQUEST,
            detail=f"Error getting planning history: {str(e)}"
        )


@router.put("/config", response_model=PathPlanningConfig)
async def update_config(
    config_data: dict,
    path_planning_service: PathPlanningService = Depends(get_path_planning_service)
):
    """Cập nhật cấu hình path planning"""
    try:
        config = await path_planning_service.update_config(config_data)
        return config
    except Exception as e:
        raise HTTPException(
            status_code=status.HTTP_400_BAD_REQUEST,
            detail=f"Error updating config: {str(e)}"
        )


@router.get("/config", response_model=PathPlanningConfig)
async def get_config(
    path_planning_service: PathPlanningService = Depends(get_path_planning_service)
):
    """Lấy cấu hình path planning hiện tại"""
    try:
        return path_planning_service.config
    except Exception as e:
        raise HTTPException(
            status_code=status.HTTP_400_BAD_REQUEST,
            detail=f"Error getting config: {str(e)}"
        )


@router.post("/validate-route")
async def validate_route(
    route: Route,
    path_planning_service: PathPlanningService = Depends(get_path_planning_service)
):
    """Validate route"""
    try:
        # Kiểm tra cơ bản
        validation_errors = []
        
        if len(route.waypoints) < 2:
            validation_errors.append("Route must have at least 2 waypoints")
        
        if route.total_distance <= 0:
            validation_errors.append("Route total distance must be positive")
        
        if route.safety_score < 0 or route.safety_score > 1:
            validation_errors.append("Safety score must be between 0 and 1")
        
        if route.efficiency_score < 0 or route.efficiency_score > 1:
            validation_errors.append("Efficiency score must be between 0 and 1")
        
        is_valid = len(validation_errors) == 0
        
        return {
            "is_valid": is_valid,
            "errors": validation_errors,
            "route_id": str(route.id),
            "total_distance": route.total_distance,
            "safety_score": route.safety_score,
            "efficiency_score": route.efficiency_score
        }
        
    except Exception as e:
        raise HTTPException(
            status_code=status.HTTP_400_BAD_REQUEST,
            detail=f"Error validating route: {str(e)}"
        )


@router.post("/simulate-route")
async def simulate_route(
    route: Route,
    path_planning_service: PathPlanningService = Depends(get_path_planning_service)
):
    """Simulate route execution"""
    try:
        # Simulate route execution
        simulation_data = {
            "route_id": str(route.id),
            "total_distance": route.total_distance,
            "estimated_time": route.total_distance / 500.0,  # Giả sử tốc độ 500mm/s
            "waypoints_count": len(route.waypoints),
            "safety_score": route.safety_score,
            "efficiency_score": route.efficiency_score,
            "simulation_steps": []
        }
        
        # Tạo simulation steps
        for i, waypoint in enumerate(route.waypoints):
            step = {
                "step": i + 1,
                "waypoint": {
                    "x": waypoint.x,
                    "y": waypoint.y,
                    "z": waypoint.z
                },
                "progress": (i / (len(route.waypoints) - 1)) * 100 if len(route.waypoints) > 1 else 100
            }
            simulation_data["simulation_steps"].append(step)
        
        return simulation_data
        
    except Exception as e:
        raise HTTPException(
            status_code=status.HTTP_400_BAD_REQUEST,
            detail=f"Error simulating route: {str(e)}"
        )
