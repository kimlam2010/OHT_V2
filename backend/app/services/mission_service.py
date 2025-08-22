"""
MissionService - Quản lý lifecycle mission
"""

import asyncio
import logging
from datetime import datetime, timedelta
from typing import List, Optional, Dict, Any
from uuid import UUID

from app.models.mission import (
    Mission, MissionCreate, MissionUpdate, MissionStatus, MissionPriority,
    ValidationResult, ExecutionStatus, MissionQueue, Point, Waypoint
)
from app.models.state import SystemState, SystemStatus
from app.services.log_service import LogService


logger = logging.getLogger(__name__)


class MissionService:
    """Service quản lý lifecycle mission"""
    
    def __init__(self, log_service: LogService):
        self.log_service = log_service
        self.mission_queue = MissionQueue()
        self.active_missions: Dict[UUID, Mission] = {}
        self.completed_missions: Dict[UUID, Mission] = {}
        self.failed_missions: Dict[UUID, Mission] = {}
        self._lock = asyncio.Lock()
        
        # Configuration
        self.max_concurrent_missions = 1
        self.mission_timeout = 3600  # 1 hour
        self.retry_delay = 5  # 5 seconds
        
        logger.info("MissionService initialized")
    
    async def create_mission(self, mission_data: dict) -> Mission:
        """Tạo mission mới"""
        async with self._lock:
            try:
                # Validate input data
                mission_create = MissionCreate(**mission_data)
                
                # Create mission
                mission = Mission(
                    name=mission_create.name,
                    description=mission_create.description,
                    priority=mission_create.priority,
                    start_point=mission_create.start_point,
                    end_point=mission_create.end_point,
                    waypoints=mission_create.waypoints,
                    max_speed=mission_create.max_speed,
                    max_acceleration=mission_create.max_acceleration,
                    max_jerk=mission_create.max_jerk,
                    tags=mission_create.tags,
                    metadata=mission_create.metadata
                )
                
                # Validate mission
                validation_result = await self.validate_mission(mission)
                if not validation_result.is_valid:
                    mission.status = MissionStatus.FAILED
                    mission.error_message = "; ".join(validation_result.errors)
                    self.failed_missions[mission.id] = mission
                    logger.error(f"Mission validation failed: {validation_result.errors}")
                    return mission
                
                # Add to queue
                if self.mission_queue.add_mission(mission):
                    mission.status = MissionStatus.QUEUED
                    logger.info(f"Mission {mission.id} created and queued")
                else:
                    mission.status = MissionStatus.FAILED
                    mission.error_message = "Queue is full"
                    self.failed_missions[mission.id] = mission
                    logger.error(f"Mission {mission.id} failed to queue: queue is full")
                
                return mission
                
            except Exception as e:
                logger.error(f"Error creating mission: {e}")
                raise
    
    async def validate_mission(self, mission: Mission) -> ValidationResult:
        """Validate mission"""
        errors = []
        warnings = []
        estimated_duration = None
        
        try:
            # Validate basic parameters
            if mission.max_speed <= 0:
                errors.append("Max speed must be positive")
            if mission.max_acceleration <= 0:
                errors.append("Max acceleration must be positive")
            if mission.max_jerk <= 0:
                errors.append("Max jerk must be positive")
            
            # Validate points
            if mission.start_point == mission.end_point:
                errors.append("Start and end points cannot be the same")
            
            # Validate waypoints
            for i, waypoint in enumerate(mission.waypoints):
                if waypoint.speed <= 0:
                    errors.append(f"Waypoint {i}: speed must be positive")
                if waypoint.dwell_time < 0:
                    errors.append(f"Waypoint {i}: dwell time cannot be negative")
            
            # Calculate estimated duration
            if not errors:
                estimated_duration = self._calculate_estimated_duration(mission)
                if estimated_duration > self.mission_timeout:
                    warnings.append(f"Estimated duration ({estimated_duration}s) exceeds timeout ({self.mission_timeout}s)")
            
            # Check queue capacity
            if len(self.mission_queue.missions) >= self.mission_queue.max_queue_size:
                warnings.append("Queue is nearly full")
            
            is_valid = len(errors) == 0
            
            return ValidationResult(
                is_valid=is_valid,
                errors=errors,
                warnings=warnings,
                estimated_duration=estimated_duration
            )
            
        except Exception as e:
            logger.error(f"Error validating mission: {e}")
            return ValidationResult(
                is_valid=False,
                errors=[f"Validation error: {str(e)}"],
                warnings=warnings,
                estimated_duration=estimated_duration
            )
    
    async def execute_mission(self, mission_id: str) -> ExecutionStatus:
        """Thực thi mission"""
        async with self._lock:
            try:
                mission_uuid = UUID(mission_id)
                
                # Find mission
                mission = None
                for m in self.mission_queue.missions:
                    if m.id == mission_uuid:
                        mission = m
                        break
                
                if not mission:
                    logger.error(f"Mission {mission_id} not found")
                    return ExecutionStatus(
                        mission_id=mission_uuid,
                        status=MissionStatus.FAILED,
                        progress=0.0,
                        current_waypoint_index=0,
                        error_message="Mission not found"
                    )
                
                # Check if system is ready
                if not await self._is_system_ready():
                    logger.warning(f"System not ready for mission {mission_id}")
                    return ExecutionStatus(
                        mission_id=mission_uuid,
                        status=MissionStatus.QUEUED,
                        progress=0.0,
                        current_waypoint_index=0,
                        error_message="System not ready"
                    )
                
                # Start execution
                mission.status = MissionStatus.EXECUTING
                mission.started_at = datetime.utcnow()
                self.active_missions[mission_uuid] = mission
                self.mission_queue.current_executing = mission_uuid
                
                # Remove from queue
                self.mission_queue.remove_mission(mission_uuid)
                
                logger.info(f"Mission {mission_id} started execution")
                
                # Start execution task
                asyncio.create_task(self._execute_mission_task(mission))
                
                return ExecutionStatus(
                    mission_id=mission_uuid,
                    status=MissionStatus.EXECUTING,
                    progress=0.0,
                    current_waypoint_index=0,
                    started_at=mission.started_at
                )
                
            except Exception as e:
                logger.error(f"Error executing mission {mission_id}: {e}")
                return ExecutionStatus(
                    mission_id=UUID(mission_id),
                    status=MissionStatus.FAILED,
                    progress=0.0,
                    current_waypoint_index=0,
                    error_message=str(e)
                )
    
    async def get_mission_status(self, mission_id: str) -> MissionStatus:
        """Lấy trạng thái mission"""
        try:
            mission_uuid = UUID(mission_id)
            
            # Check active missions
            if mission_uuid in self.active_missions:
                return self.active_missions[mission_uuid].status
            
            # Check completed missions
            if mission_uuid in self.completed_missions:
                return self.completed_missions[mission_uuid].status
            
            # Check failed missions
            if mission_uuid in self.failed_missions:
                return self.failed_missions[mission_uuid].status
            
            # Check queue
            for mission in self.mission_queue.missions:
                if mission.id == mission_uuid:
                    return mission.status
            
            return MissionStatus.FAILED
            
        except Exception as e:
            logger.error(f"Error getting mission status {mission_id}: {e}")
            return MissionStatus.FAILED
    
    async def cancel_mission(self, mission_id: str) -> bool:
        """Hủy mission"""
        async with self._lock:
            try:
                mission_uuid = UUID(mission_id)
                
                # Check active missions
                if mission_uuid in self.active_missions:
                    mission = self.active_missions[mission_uuid]
                    mission.status = MissionStatus.CANCELLED
                    mission.completed_at = datetime.utcnow()
                    self.completed_missions[mission_uuid] = mission
                    del self.active_missions[mission_uuid]
                    
                    # Clear current executing
                    if self.mission_queue.current_executing == mission_uuid:
                        self.mission_queue.current_executing = None
                    
                    logger.info(f"Mission {mission_id} cancelled")
                    return True
                
                # Check queue
                if self.mission_queue.remove_mission(mission_uuid):
                    logger.info(f"Mission {mission_id} removed from queue")
                    return True
                
                logger.warning(f"Mission {mission_id} not found for cancellation")
                return False
                
            except Exception as e:
                logger.error(f"Error cancelling mission {mission_id}: {e}")
                return False
    
    async def get_mission_queue(self) -> List[Mission]:
        """Lấy danh sách mission trong queue"""
        return self.mission_queue.missions.copy()
    
    async def get_active_missions(self) -> List[Mission]:
        """Lấy danh sách mission đang thực thi"""
        return list(self.active_missions.values())
    
    async def get_completed_missions(self) -> List[Mission]:
        """Lấy danh sách mission đã hoàn thành"""
        return list(self.completed_missions.values())
    
    async def get_failed_missions(self) -> List[Mission]:
        """Lấy danh sách mission thất bại"""
        return list(self.failed_missions.values())
    
    async def update_mission(self, mission_id: str, update_data: dict) -> Optional[Mission]:
        """Cập nhật mission"""
        async with self._lock:
            try:
                mission_uuid = UUID(mission_id)
                
                # Find mission in queue
                for mission in self.mission_queue.missions:
                    if mission.id == mission_uuid:
                        mission_update = MissionUpdate(**update_data)
                        
                        # Update fields
                        for field, value in mission_update.dict(exclude_unset=True).items():
                            setattr(mission, field, value)
                        
                        # Re-validate if needed
                        if mission.status == MissionStatus.CREATED:
                            validation_result = await self.validate_mission(mission)
                            if validation_result.is_valid:
                                mission.status = MissionStatus.VALIDATED
                            else:
                                mission.status = MissionStatus.FAILED
                                mission.error_message = "; ".join(validation_result.errors)
                        
                        logger.info(f"Mission {mission_id} updated")
                        return mission
                
                logger.warning(f"Mission {mission_id} not found for update")
                return None
                
            except Exception as e:
                logger.error(f"Error updating mission {mission_id}: {e}")
                return None
    
    async def _execute_mission_task(self, mission: Mission):
        """Task thực thi mission"""
        try:
            logger.info(f"Starting execution task for mission {mission.id}")
            
            # Simulate mission execution
            total_waypoints = len(mission.waypoints) + 2  # start + waypoints + end
            current_waypoint = 0
            
            # Move to start point
            await self._move_to_point(mission.start_point, mission.max_speed)
            current_waypoint += 1
            mission.progress = (current_waypoint / total_waypoints) * 100
            mission.current_waypoint_index = current_waypoint - 1
            
            # Execute waypoints
            for i, waypoint in enumerate(mission.waypoints):
                await self._move_to_point(waypoint.point, waypoint.speed)
                
                # Dwell time
                if waypoint.dwell_time > 0:
                    await asyncio.sleep(waypoint.dwell_time)
                
                current_waypoint += 1
                mission.progress = (current_waypoint / total_waypoints) * 100
                mission.current_waypoint_index = i + 1
            
            # Move to end point
            await self._move_to_point(mission.end_point, mission.max_speed)
            current_waypoint += 1
            mission.progress = 100.0
            mission.current_waypoint_index = len(mission.waypoints) + 1
            
            # Complete mission
            mission.status = MissionStatus.COMPLETED
            mission.completed_at = datetime.utcnow()
            
            # Move to completed missions
            self.completed_missions[mission.id] = mission
            if mission.id in self.active_missions:
                del self.active_missions[mission.id]
            
            # Clear current executing
            if self.mission_queue.current_executing == mission.id:
                self.mission_queue.current_executing = None
            
            logger.info(f"Mission {mission.id} completed successfully")
            
        except Exception as e:
            logger.error(f"Error in mission execution task {mission.id}: {e}")
            mission.status = MissionStatus.FAILED
            mission.error_message = str(e)
            mission.completed_at = datetime.utcnow()
            
            # Move to failed missions
            self.failed_missions[mission.id] = mission
            if mission.id in self.active_missions:
                del self.active_missions[mission.id]
            
            # Clear current executing
            if self.mission_queue.current_executing == mission.id:
                self.mission_queue.current_executing = None
    
    async def _move_to_point(self, point: Point, speed: float):
        """Di chuyển đến điểm (simulation)"""
        # Simulate movement time based on distance and speed
        distance = point.distance_to(Point(x=0, y=0, z=0))  # Assume starting from origin
        movement_time = distance / speed if speed > 0 else 1.0
        
        await asyncio.sleep(movement_time)
    
    def _calculate_estimated_duration(self, mission: Mission) -> float:
        """Tính thời gian ước tính cho mission"""
        total_distance = mission.start_point.distance_to(mission.end_point)
        
        # Add waypoint distances
        current_point = mission.start_point
        for waypoint in mission.waypoints:
            total_distance += current_point.distance_to(waypoint.point)
            current_point = waypoint.point
        total_distance += current_point.distance_to(mission.end_point)
        
        # Add dwell times
        total_dwell_time = sum(w.dwell_time for w in mission.waypoints)
        
        # Calculate movement time (simplified)
        movement_time = total_distance / mission.max_speed
        
        return movement_time + total_dwell_time
    
    async def _is_system_ready(self) -> bool:
        """Kiểm tra hệ thống có sẵn sàng không"""
        # TODO: Integrate with StateManagementService
        return True
    
    async def cleanup_old_missions(self, max_age_hours: int = 24):
        """Dọn dẹp mission cũ"""
        async with self._lock:
            cutoff_time = datetime.utcnow() - timedelta(hours=max_age_hours)
            
            # Clean completed missions
            old_completed = [
                mission_id for mission_id, mission in self.completed_missions.items()
                if mission.completed_at and mission.completed_at < cutoff_time
            ]
            for mission_id in old_completed:
                del self.completed_missions[mission_id]
            
            # Clean failed missions
            old_failed = [
                mission_id for mission_id, mission in self.failed_missions.items()
                if mission.completed_at and mission.completed_at < cutoff_time
            ]
            for mission_id in old_failed:
                del self.failed_missions[mission_id]
            
            if old_completed or old_failed:
                logger.info(f"Cleaned up {len(old_completed)} completed and {len(old_failed)} failed missions")
