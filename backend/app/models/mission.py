from datetime import datetime
from enum import Enum
from typing import List, Optional, Dict, Any
from pydantic import BaseModel, Field
from uuid import UUID, uuid4


class MissionStatus(str, Enum):
    """Trạng thái của mission"""
    CREATED = "created"
    VALIDATED = "validated"
    QUEUED = "queued"
    EXECUTING = "executing"
    COMPLETED = "completed"
    FAILED = "failed"
    CANCELLED = "cancelled"


class MissionPriority(str, Enum):
    """Độ ưu tiên của mission"""
    LOW = "low"
    NORMAL = "normal"
    HIGH = "high"
    CRITICAL = "critical"


class Point(BaseModel):
    """Điểm trong không gian 3D"""
    x: float = Field(..., description="Tọa độ X (mm)")
    y: float = Field(..., description="Tọa độ Y (mm)")
    z: float = Field(0.0, description="Tọa độ Z (mm)")
    
    def distance_to(self, other: 'Point') -> float:
        """Tính khoảng cách đến điểm khác"""
        import math
        return math.sqrt((self.x - other.x)**2 + (self.y - other.y)**2 + (self.z - other.z)**2)


class Waypoint(BaseModel):
    """Điểm dừng trong mission"""
    point: Point
    speed: float = Field(..., description="Tốc độ tại điểm này (mm/s)")
    dwell_time: float = Field(0.0, description="Thời gian dừng (s)")
    action: Optional[str] = Field(None, description="Hành động tại điểm này")


class Mission(BaseModel):
    """Model cho mission"""
    id: UUID = Field(default_factory=uuid4, description="ID duy nhất của mission")
    name: str = Field(..., description="Tên mission")
    description: Optional[str] = Field(None, description="Mô tả mission")
    priority: MissionPriority = Field(MissionPriority.NORMAL, description="Độ ưu tiên")
    status: MissionStatus = Field(MissionStatus.CREATED, description="Trạng thái hiện tại")
    
    # Điểm bắt đầu và kết thúc
    start_point: Point = Field(..., description="Điểm bắt đầu")
    end_point: Point = Field(..., description="Điểm kết thúc")
    waypoints: List[Waypoint] = Field(default_factory=list, description="Danh sách điểm dừng")
    
    # Thông số chuyển động
    max_speed: float = Field(..., description="Tốc độ tối đa (mm/s)")
    max_acceleration: float = Field(..., description="Gia tốc tối đa (mm/s²)")
    max_jerk: float = Field(..., description="Jerk tối đa (mm/s³)")
    
    # Thời gian
    created_at: datetime = Field(default_factory=datetime.utcnow, description="Thời gian tạo")
    started_at: Optional[datetime] = Field(None, description="Thời gian bắt đầu")
    completed_at: Optional[datetime] = Field(None, description="Thời gian hoàn thành")
    estimated_duration: Optional[float] = Field(None, description="Thời gian ước tính (s)")
    
    # Metadata
    tags: List[str] = Field(default_factory=list, description="Tags cho mission")
    metadata: Dict[str, Any] = Field(default_factory=dict, description="Metadata bổ sung")
    
    # Progress tracking
    progress: float = Field(0.0, description="Tiến độ (0-100%)")
    current_waypoint_index: int = Field(0, description="Index của waypoint hiện tại")
    
    # Error handling
    error_message: Optional[str] = Field(None, description="Thông báo lỗi nếu có")
    retry_count: int = Field(0, description="Số lần thử lại")
    max_retries: int = Field(3, description="Số lần thử lại tối đa")


class MissionCreate(BaseModel):
    """Model để tạo mission mới"""
    name: str
    description: Optional[str] = None
    priority: MissionPriority = MissionPriority.NORMAL
    start_point: Point
    end_point: Point
    waypoints: List[Waypoint] = []
    max_speed: float
    max_acceleration: float
    max_jerk: float
    tags: List[str] = []
    metadata: Dict[str, Any] = {}


class MissionUpdate(BaseModel):
    """Model để cập nhật mission"""
    name: Optional[str] = None
    description: Optional[str] = None
    priority: Optional[MissionPriority] = None
    waypoints: Optional[List[Waypoint]] = None
    max_speed: Optional[float] = None
    max_acceleration: Optional[float] = None
    max_jerk: Optional[float] = None
    tags: Optional[List[str]] = None
    metadata: Optional[Dict[str, Any]] = None


class ValidationResult(BaseModel):
    """Kết quả validation mission"""
    is_valid: bool = Field(..., description="Mission có hợp lệ không")
    errors: List[str] = Field(default_factory=list, description="Danh sách lỗi")
    warnings: List[str] = Field(default_factory=list, description="Danh sách cảnh báo")
    estimated_duration: Optional[float] = Field(None, description="Thời gian ước tính")


class ExecutionStatus(BaseModel):
    """Trạng thái thực thi mission"""
    mission_id: UUID = Field(..., description="ID của mission")
    status: MissionStatus = Field(..., description="Trạng thái hiện tại")
    progress: float = Field(..., description="Tiến độ (0-100%)")
    current_position: Optional[Point] = Field(None, description="Vị trí hiện tại")
    current_waypoint_index: int = Field(..., description="Index waypoint hiện tại")
    error_message: Optional[str] = Field(None, description="Thông báo lỗi")
    started_at: Optional[datetime] = Field(None, description="Thời gian bắt đầu")
    estimated_completion: Optional[datetime] = Field(None, description="Thời gian hoàn thành ước tính")


class MissionQueue(BaseModel):
    """Queue quản lý các mission"""
    queue_id: UUID = Field(default_factory=uuid4, description="ID của queue")
    missions: List[Mission] = Field(default_factory=list, description="Danh sách mission trong queue")
    max_queue_size: int = Field(100, description="Kích thước tối đa của queue")
    current_executing: Optional[UUID] = Field(None, description="ID mission đang thực thi")
    
    def add_mission(self, mission: Mission) -> bool:
        """Thêm mission vào queue"""
        if len(self.missions) >= self.max_queue_size:
            return False
        self.missions.append(mission)
        return True
    
    def remove_mission(self, mission_id: UUID) -> bool:
        """Xóa mission khỏi queue"""
        for i, mission in enumerate(self.missions):
            if mission.id == mission_id:
                self.missions.pop(i)
                return True
        return False
    
    def get_next_mission(self) -> Optional[Mission]:
        """Lấy mission tiếp theo theo độ ưu tiên"""
        if not self.missions:
            return None
        
        # Sắp xếp theo độ ưu tiên và thời gian tạo
        sorted_missions = sorted(
            self.missions,
            key=lambda m: (m.priority.value, m.created_at)
        )
        return sorted_missions[0] if sorted_missions else None
