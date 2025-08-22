from datetime import datetime
from enum import Enum
from typing import List, Optional, Dict, Any
from pydantic import BaseModel, Field
from uuid import UUID, uuid4
import math


class MovementStatus(str, Enum):
    """Trạng thái chuyển động"""
    IDLE = "idle"
    PLANNING = "planning"
    EXECUTING = "executing"
    PAUSED = "paused"
    COMPLETED = "completed"
    FAILED = "failed"
    EMERGENCY_STOP = "emergency_stop"


class TrajectoryType(str, Enum):
    """Loại quỹ đạo"""
    LINEAR = "linear"
    S_CURVE = "s_curve"
    TRAPEZOIDAL = "trapezoidal"
    CUSTOM = "custom"


class Point(BaseModel):
    """Điểm trong không gian 3D"""
    x: float = Field(..., description="Tọa độ X (mm)")
    y: float = Field(..., description="Tọa độ Y (mm)")
    z: float = Field(0.0, description="Tọa độ Z (mm)")
    
    def distance_to(self, other: 'Point') -> float:
        """Tính khoảng cách đến điểm khác"""
        return math.sqrt(
            (self.x - other.x) ** 2 + 
            (self.y - other.y) ** 2 + 
            (self.z - other.z) ** 2
        )


class SpeedProfile(BaseModel):
    """Profile tốc độ"""
    max_speed: float = Field(..., description="Tốc độ tối đa (mm/s)")
    max_acceleration: float = Field(..., description="Gia tốc tối đa (mm/s²)")
    max_jerk: float = Field(..., description="Jerk tối đa (mm/s³)")
    start_speed: float = Field(0.0, description="Tốc độ bắt đầu (mm/s)")
    end_speed: float = Field(0.0, description="Tốc độ kết thúc (mm/s)")
    cruise_speed: Optional[float] = Field(None, description="Tốc độ hành trình (mm/s)")


class TrajectorySegment(BaseModel):
    """Đoạn quỹ đạo"""
    id: UUID = Field(default_factory=uuid4, description="ID duy nhất")
    start_point: Point = Field(..., description="Điểm bắt đầu")
    end_point: Point = Field(..., description="Điểm kết thúc")
    segment_type: str = Field(..., description="Loại đoạn (acceleration, cruise, deceleration)")
    duration: float = Field(..., description="Thời gian thực hiện (s)")
    distance: float = Field(..., description="Khoảng cách (mm)")
    start_time: float = Field(0.0, description="Thời gian bắt đầu (s)")
    end_time: float = Field(..., description="Thời gian kết thúc (s)")
    speed_profile: SpeedProfile = Field(..., description="Profile tốc độ")


class Trajectory(BaseModel):
    """Quỹ đạo chuyển động"""
    id: UUID = Field(default_factory=uuid4, description="ID duy nhất")
    trajectory_type: TrajectoryType = Field(..., description="Loại quỹ đạo")
    start_point: Point = Field(..., description="Điểm bắt đầu")
    end_point: Point = Field(..., description="Điểm kết thúc")
    waypoints: List[Point] = Field(default_factory=list, description="Điểm dừng")
    segments: List[TrajectorySegment] = Field(default_factory=list, description="Các đoạn quỹ đạo")
    total_distance: float = Field(0.0, description="Tổng khoảng cách (mm)")
    total_duration: float = Field(0.0, description="Tổng thời gian (s)")
    speed_profile: SpeedProfile = Field(..., description="Profile tốc độ tổng thể")
    created_at: datetime = Field(default_factory=datetime.utcnow, description="Thời gian tạo")
    is_valid: bool = Field(True, description="Quỹ đạo có hợp lệ không")
    validation_errors: List[str] = Field(default_factory=list, description="Lỗi validation")


class Position(BaseModel):
    """Vị trí hiện tại"""
    x: float = Field(..., description="Tọa độ X (mm)")
    y: float = Field(..., description="Tọa độ Y (mm)")
    z: float = Field(0.0, description="Tọa độ Z (mm)")
    timestamp: datetime = Field(default_factory=datetime.utcnow, description="Thời gian đo")
    accuracy: float = Field(1.0, description="Độ chính xác (mm)")
    source: str = Field("encoder", description="Nguồn dữ liệu")


class MovementStatus(BaseModel):
    """Trạng thái chuyển động chi tiết"""
    status: MovementStatus = Field(MovementStatus.IDLE, description="Trạng thái hiện tại")
    current_position: Position = Field(..., description="Vị trí hiện tại")
    target_position: Optional[Position] = Field(None, description="Vị trí mục tiêu")
    current_speed: float = Field(0.0, description="Tốc độ hiện tại (mm/s)")
    current_acceleration: float = Field(0.0, description="Gia tốc hiện tại (mm/s²)")
    progress: float = Field(0.0, description="Tiến độ (0-100%)")
    remaining_distance: float = Field(0.0, description="Khoảng cách còn lại (mm)")
    estimated_completion: Optional[datetime] = Field(None, description="Thời gian hoàn thành ước tính")
    error_message: Optional[str] = Field(None, description="Thông báo lỗi")
    last_update: datetime = Field(default_factory=datetime.utcnow, description="Thời gian cập nhật cuối")


class MotorCommand(BaseModel):
    """Lệnh điều khiển motor"""
    id: UUID = Field(default_factory=uuid4, description="ID duy nhất")
    motor_id: str = Field(..., description="ID của motor")
    command_type: str = Field(..., description="Loại lệnh (move, stop, home, etc.)")
    target_position: Optional[float] = Field(None, description="Vị trí mục tiêu")
    target_speed: Optional[float] = Field(None, description="Tốc độ mục tiêu")
    target_acceleration: Optional[float] = Field(None, description="Gia tốc mục tiêu")
    parameters: Dict[str, Any] = Field(default_factory=dict, description="Tham số bổ sung")
    timestamp: datetime = Field(default_factory=datetime.utcnow, description="Thời gian tạo")
    priority: int = Field(0, description="Độ ưu tiên (cao hơn = ưu tiên hơn)")
    is_executed: bool = Field(False, description="Đã thực hiện chưa")
    execution_time: Optional[datetime] = Field(None, description="Thời gian thực hiện")


class PIDController(BaseModel):
    """Bộ điều khiển PID"""
    kp: float = Field(1.0, description="Hệ số tỷ lệ")
    ki: float = Field(0.0, description="Hệ số tích phân")
    kd: float = Field(0.0, description="Hệ số vi phân")
    setpoint: float = Field(0.0, description="Giá trị đặt")
    output_min: float = Field(-100.0, description="Giá trị đầu ra tối thiểu")
    output_max: float = Field(100.0, description="Giá trị đầu ra tối đa")
    integral_limit: float = Field(100.0, description="Giới hạn tích phân")
    deadband: float = Field(0.0, description="Vùng chết")
    is_enabled: bool = Field(True, description="Bộ điều khiển có đang hoạt động không")


class MovementConfig(BaseModel):
    """Cấu hình chuyển động"""
    max_speed: float = Field(1000.0, description="Tốc độ tối đa (mm/s)")
    max_acceleration: float = Field(500.0, description="Gia tốc tối đa (mm/s²)")
    max_jerk: float = Field(1000.0, description="Jerk tối đa (mm/s³)")
    position_tolerance: float = Field(1.0, description="Dung sai vị trí (mm)")
    speed_tolerance: float = Field(1.0, description="Dung sai tốc độ (mm/s)")
    control_frequency: float = Field(100.0, description="Tần số điều khiển (Hz)")
    trajectory_planning_enabled: bool = Field(True, description="Bật lập kế hoạch quỹ đạo")
    pid_control_enabled: bool = Field(True, description="Bật điều khiển PID")
    emergency_stop_enabled: bool = Field(True, description="Bật dừng khẩn cấp")


class MovementEvent(BaseModel):
    """Sự kiện chuyển động"""
    id: UUID = Field(default_factory=uuid4, description="ID duy nhất")
    timestamp: datetime = Field(default_factory=datetime.utcnow, description="Thời gian xảy ra")
    event_type: str = Field(..., description="Loại sự kiện")
    description: str = Field(..., description="Mô tả sự kiện")
    position: Optional[Position] = Field(None, description="Vị trí tại thời điểm sự kiện")
    speed: Optional[float] = Field(None, description="Tốc độ tại thời điểm sự kiện")
    data: Dict[str, Any] = Field(default_factory=dict, description="Dữ liệu bổ sung")
    severity: str = Field("info", description="Mức độ nghiêm trọng")
