"""
MovementService - Điều khiển chuyển động
"""

import asyncio
import logging
import math
from datetime import datetime, timedelta
from typing import List, Optional, Dict
from uuid import UUID

from app.models.movement import (
    Trajectory, TrajectoryType, SpeedProfile, Position, MovementStatus,
    MotorCommand, PIDController, MovementConfig, MovementEvent
)
from app.models.mission import Point
from app.services.log_service import LogService


logger = logging.getLogger(__name__)


class MovementService:
    """Service điều khiển chuyển động"""
    
    def __init__(self, log_service: LogService):
        self.log_service = log_service
        self.movement_config = MovementConfig()
        self.current_position = Position(x=0, y=0, z=0)
        self.current_speed = 0.0
        self.current_acceleration = 0.0
        self.movement_status = MovementStatus.IDLE
        self.active_trajectory: Optional[Trajectory] = None
        self.pid_controllers: Dict[str, PIDController] = {}
        self.motor_commands: List[MotorCommand] = []
        self.movement_events: List[MovementEvent] = []
        self._lock = asyncio.Lock()
        
        # Initialize PID controllers
        self._initialize_pid_controllers()
        
        logger.info("MovementService initialized")
    
    async def plan_trajectory(self, start: Point, end: Point) -> Trajectory:
        """Lập kế hoạch quỹ đạo"""
        async with self._lock:
            try:
                # Calculate distance
                distance = start.distance_to(end)
                
                # Create speed profile
                speed_profile = SpeedProfile(
                    max_speed=self.movement_config.max_speed,
                    max_acceleration=self.movement_config.max_acceleration,
                    max_jerk=self.movement_config.max_jerk,
                    start_speed=0.0,
                    end_speed=0.0
                )
                
                # Calculate trajectory parameters
                if distance > 0:
                    # Simple trapezoidal profile
                    acceleration_distance = (speed_profile.max_speed ** 2) / (2 * speed_profile.max_acceleration)
                    cruise_distance = distance - 2 * acceleration_distance
                    
                    if cruise_distance > 0:
                        # Trapezoidal profile
                        trajectory_type = TrajectoryType.TRAPEZOIDAL
                        total_time = (2 * speed_profile.max_speed / speed_profile.max_acceleration) + (cruise_distance / speed_profile.max_speed)
                    else:
                        # Triangular profile
                        trajectory_type = TrajectoryType.TRAPEZOIDAL
                        speed_profile.max_speed = math.sqrt(distance * speed_profile.max_acceleration)
                        total_time = 2 * math.sqrt(distance / speed_profile.max_acceleration)
                else:
                    # No movement needed
                    trajectory_type = TrajectoryType.LINEAR
                    total_time = 0.0
                
                # Create trajectory
                trajectory = Trajectory(
                    trajectory_type=trajectory_type,
                    start_point=start,
                    end_point=end,
                    total_distance=distance,
                    total_duration=total_time,
                    speed_profile=speed_profile
                )
                
                logger.info(f"Trajectory planned: distance={distance}mm, duration={total_time}s")
                return trajectory
                
            except Exception as e:
                logger.error(f"Error planning trajectory: {e}")
                raise
    
    async def execute_movement(self, trajectory: Trajectory) -> MovementStatus:
        """Thực hiện chuyển động"""
        async with self._lock:
            try:
                if self.movement_status != MovementStatus.IDLE:
                    logger.warning("Movement already in progress")
                    return self.movement_status
                
                # Validate trajectory
                if not trajectory.is_valid:
                    logger.error("Invalid trajectory")
                    return MovementStatus.FAILED
                
                # Start movement
                self.movement_status = MovementStatus.EXECUTING
                self.active_trajectory = trajectory
                
                # Create movement event
                event = MovementEvent(
                    event_type="movement_started",
                    description=f"Movement started: {trajectory.start_point} -> {trajectory.end_point}",
                    position=self.current_position,
                    speed=self.current_speed,
                    severity="info"
                )
                self.movement_events.append(event)
                
                # Start movement task
                asyncio.create_task(self._execute_movement_task(trajectory))
                
                logger.info(f"Movement started: {trajectory.id}")
                return MovementStatus.EXECUTING
                
            except Exception as e:
                logger.error(f"Error executing movement: {e}")
                self.movement_status = MovementStatus.FAILED
                return MovementStatus.FAILED
    
    async def control_speed(self, profile: SpeedProfile) -> bool:
        """Điều khiển tốc độ"""
        async with self._lock:
            try:
                # Validate speed profile
                if profile.max_speed > self.movement_config.max_speed:
                    logger.warning(f"Speed {profile.max_speed} exceeds max speed {self.movement_config.max_speed}")
                    return False
                
                # Update current speed
                self.current_speed = profile.max_speed
                
                # Create motor command
                command = MotorCommand(
                    motor_id="main_motor",
                    command_type="set_speed",
                    target_speed=profile.max_speed,
                    target_acceleration=profile.max_acceleration,
                    priority=1
                )
                self.motor_commands.append(command)
                
                logger.info(f"Speed controlled: {profile.max_speed} mm/s")
                return True
                
            except Exception as e:
                logger.error(f"Error controlling speed: {e}")
                return False
    
    async def get_position(self) -> Position:
        """Lấy vị trí hiện tại"""
        async with self._lock:
            return self.current_position
    
    async def stop_movement(self) -> bool:
        """Dừng chuyển động"""
        async with self._lock:
            try:
                if self.movement_status == MovementStatus.IDLE:
                    logger.info("No movement to stop")
                    return True
                
                # Stop movement
                self.movement_status = MovementStatus.IDLE
                self.current_speed = 0.0
                self.current_acceleration = 0.0
                self.active_trajectory = None
                
                # Create stop command
                command = MotorCommand(
                    motor_id="main_motor",
                    command_type="stop",
                    priority=10  # High priority for stop command
                )
                self.motor_commands.append(command)
                
                # Create movement event
                event = MovementEvent(
                    event_type="movement_stopped",
                    description="Movement stopped by user",
                    position=self.current_position,
                    speed=0.0,
                    severity="info"
                )
                self.movement_events.append(event)
                
                logger.info("Movement stopped")
                return True
                
            except Exception as e:
                logger.error(f"Error stopping movement: {e}")
                return False
    
    async def get_movement_status(self) -> MovementStatus:
        """Lấy trạng thái chuyển động chi tiết"""
        async with self._lock:
            return MovementStatus(
                status=self.movement_status,
                current_position=self.current_position,
                target_position=Position(
                    x=self.active_trajectory.end_point.x,
                    y=self.active_trajectory.end_point.y,
                    z=self.active_trajectory.end_point.z
                ) if self.active_trajectory else None,
                current_speed=self.current_speed,
                current_acceleration=self.current_acceleration,
                progress=self._calculate_progress(),
                remaining_distance=self._calculate_remaining_distance(),
                estimated_completion=self._calculate_estimated_completion(),
                last_update=datetime.utcnow()
            )
    
    async def update_position(self, position: Position):
        """Cập nhật vị trí (từ encoder/sensor)"""
        async with self._lock:
            self.current_position = position
    
    async def emergency_stop(self) -> bool:
        """Dừng khẩn cấp"""
        async with self._lock:
            try:
                # Immediate stop
                self.movement_status = MovementStatus.EMERGENCY_STOP
                self.current_speed = 0.0
                self.current_acceleration = 0.0
                self.active_trajectory = None
                
                # Create emergency stop command
                command = MotorCommand(
                    motor_id="main_motor",
                    command_type="emergency_stop",
                    priority=100  # Highest priority
                )
                self.motor_commands.append(command)
                
                # Create movement event
                event = MovementEvent(
                    event_type="emergency_stop",
                    description="Emergency stop activated",
                    position=self.current_position,
                    speed=0.0,
                    severity="critical"
                )
                self.movement_events.append(event)
                
                logger.warning("EMERGENCY STOP ACTIVATED")
                return True
                
            except Exception as e:
                logger.error(f"Error in emergency stop: {e}")
                return False
    
    async def get_movement_events(self, limit: int = 100) -> List[MovementEvent]:
        """Lấy sự kiện chuyển động"""
        async with self._lock:
            return self.movement_events[-limit:]
    
    async def update_movement_config(self, config_data: dict) -> MovementConfig:
        """Cập nhật cấu hình chuyển động"""
        async with self._lock:
            try:
                for key, value in config_data.items():
                    if hasattr(self.movement_config, key):
                        setattr(self.movement_config, key, value)
                
                logger.info("Movement configuration updated")
                return self.movement_config
                
            except Exception as e:
                logger.error(f"Error updating movement config: {e}")
                raise
    
    async def _execute_movement_task(self, trajectory: Trajectory):
        """Task thực hiện chuyển động"""
        try:
            logger.info(f"Starting movement task for trajectory {trajectory.id}")
            
            # Simulate movement execution
            start_time = datetime.utcnow()
            total_distance = trajectory.total_distance
            current_distance = 0.0
            
            while current_distance < total_distance and self.movement_status == MovementStatus.EXECUTING:
                # Calculate progress
                progress = current_distance / total_distance
                
                # Update position (simulate movement)
                dx = (trajectory.end_point.x - trajectory.start_point.x) * progress
                dy = (trajectory.end_point.y - trajectory.start_point.y) * progress
                
                new_position = Position(
                    x=trajectory.start_point.x + dx,
                    y=trajectory.start_point.y + dy,
                    z=trajectory.start_point.z
                )
                
                await self.update_position(new_position)
                
                # Update speed based on trajectory profile
                if progress < 0.1:
                    # Acceleration phase
                    self.current_speed = trajectory.speed_profile.max_speed * (progress / 0.1)
                    self.current_acceleration = trajectory.speed_profile.max_acceleration
                elif progress > 0.9:
                    # Deceleration phase
                    self.current_speed = trajectory.speed_profile.max_speed * ((1 - progress) / 0.1)
                    self.current_acceleration = -trajectory.speed_profile.max_acceleration
                else:
                    # Cruise phase
                    self.current_speed = trajectory.speed_profile.max_speed
                    self.current_acceleration = 0.0
                
                # Simulate movement time
                await asyncio.sleep(0.1)  # 100ms update interval
                
                current_distance += self.current_speed * 0.1
            
            # Movement completed
            if self.movement_status == MovementStatus.EXECUTING:
                self.movement_status = MovementStatus.COMPLETED
                
                # Create completion event
                event = MovementEvent(
                    event_type="movement_completed",
                    description=f"Movement completed: {trajectory.start_point} -> {trajectory.end_point}",
                    position=self.current_position,
                    speed=0.0,
                    severity="info"
                )
                self.movement_events.append(event)
                
                logger.info(f"Movement completed: {trajectory.id}")
            
        except Exception as e:
            logger.error(f"Error in movement task {trajectory.id}: {e}")
            self.movement_status = MovementStatus.FAILED
    
    def _initialize_pid_controllers(self):
        """Khởi tạo bộ điều khiển PID"""
        # X-axis controller
        self.pid_controllers["x"] = PIDController(
            kp=1.0,
            ki=0.1,
            kd=0.05,
            setpoint=0.0
        )
        
        # Y-axis controller
        self.pid_controllers["y"] = PIDController(
            kp=1.0,
            ki=0.1,
            kd=0.05,
            setpoint=0.0
        )
        
        # Z-axis controller
        self.pid_controllers["z"] = PIDController(
            kp=1.0,
            ki=0.1,
            kd=0.05,
            setpoint=0.0
        )
    
    def _calculate_progress(self) -> float:
        """Tính tiến độ chuyển động"""
        if not self.active_trajectory:
            return 0.0
        
        current_distance = self.current_position.distance_to(Point(
            x=self.active_trajectory.start_point.x,
            y=self.active_trajectory.start_point.y,
            z=self.active_trajectory.start_point.z
        ))
        
        return min(current_distance / self.active_trajectory.total_distance, 1.0) * 100
    
    def _calculate_remaining_distance(self) -> float:
        """Tính khoảng cách còn lại"""
        if not self.active_trajectory:
            return 0.0
        
        current_distance = self.current_position.distance_to(Point(
            x=self.active_trajectory.start_point.x,
            y=self.active_trajectory.start_point.y,
            z=self.active_trajectory.start_point.z
        ))
        
        return max(self.active_trajectory.total_distance - current_distance, 0.0)
    
    def _calculate_estimated_completion(self) -> Optional[datetime]:
        """Tính thời gian hoàn thành ước tính"""
        if not self.active_trajectory or self.current_speed <= 0:
            return None
        
        remaining_distance = self._calculate_remaining_distance()
        remaining_time = remaining_distance / self.current_speed
        
        return datetime.utcnow() + timedelta(seconds=remaining_time)
