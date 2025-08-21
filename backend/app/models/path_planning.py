"""
Path Planning Models cho OHT-50 Backend
"""
from datetime import datetime
from enum import Enum
from typing import List, Optional, Dict, Any
from pydantic import BaseModel, Field
from uuid import UUID, uuid4
import math

from .mission import Point


class RouteType(str, Enum):
    """Loại route"""
    LINEAR = "linear"
    OPTIMIZED = "optimized"
    SAFE = "safe"
    EMERGENCY = "emergency"


class ObstacleType(str, Enum):
    """Loại chướng ngại vật"""
    STATIC = "static"
    DYNAMIC = "dynamic"
    TEMPORARY = "temporary"
    SAFETY_ZONE = "safety_zone"


class Obstacle(BaseModel):
    """Chướng ngại vật"""
    id: UUID = Field(default_factory=uuid4, description="ID duy nhất")
    obstacle_type: ObstacleType = Field(..., description="Loại chướng ngại vật")
    position: Point = Field(..., description="Vị trí chướng ngại vật")
    radius: float = Field(..., description="Bán kính ảnh hưởng (mm)")
    confidence: float = Field(1.0, ge=0.0, le=1.0, description="Độ tin cậy (0-1)")
    timestamp: datetime = Field(default_factory=datetime.utcnow, description="Thời gian phát hiện")
    is_active: bool = Field(True, description="Chướng ngại vật có đang hoạt động không")
    metadata: Dict[str, Any] = Field(default_factory=dict, description="Metadata bổ sung")


class RouteSegment(BaseModel):
    """Đoạn route"""
    id: UUID = Field(default_factory=uuid4, description="ID duy nhất")
    start_point: Point = Field(..., description="Điểm bắt đầu")
    end_point: Point = Field(..., description="Điểm kết thúc")
    segment_type: str = Field(..., description="Loại đoạn")
    distance: float = Field(..., description="Khoảng cách (mm)")
    estimated_time: float = Field(..., description="Thời gian ước tính (s)")
    speed_limit: float = Field(..., description="Giới hạn tốc độ (mm/s)")
    safety_margin: float = Field(50.0, description="Khoảng cách an toàn (mm)")
    obstacles: List[Obstacle] = Field(default_factory=list, description="Chướng ngại vật trên đoạn")
    is_safe: bool = Field(True, description="Đoạn có an toàn không")


class Route(BaseModel):
    """Route hoàn chỉnh"""
    id: UUID = Field(default_factory=uuid4, description="ID duy nhất")
    route_type: RouteType = Field(..., description="Loại route")
    waypoints: List[Point] = Field(..., description="Danh sách điểm dừng")
    segments: List[RouteSegment] = Field(default_factory=list, description="Các đoạn route")
    total_distance: float = Field(0.0, description="Tổng khoảng cách (mm)")
    total_time: float = Field(0.0, description="Tổng thời gian (s)")
    safety_score: float = Field(1.0, ge=0.0, le=1.0, description="Điểm an toàn (0-1)")
    efficiency_score: float = Field(1.0, ge=0.0, le=1.0, description="Điểm hiệu quả (0-1)")
    obstacles_avoided: int = Field(0, description="Số chướng ngại vật tránh được")
    created_at: datetime = Field(default_factory=datetime.utcnow, description="Thời gian tạo")
    is_valid: bool = Field(True, description="Route có hợp lệ không")
    validation_errors: List[str] = Field(default_factory=list, description="Lỗi validation")


class SafeRoute(Route):
    """Route an toàn với validation"""
    safety_zones_integrated: bool = Field(True, description="Đã tích hợp vùng an toàn")
    emergency_exits: List[Point] = Field(default_factory=list, description="Điểm thoát khẩn cấp")
    backup_routes: List[Route] = Field(default_factory=list, description="Route dự phòng")
    risk_assessment: Dict[str, float] = Field(default_factory=dict, description="Đánh giá rủi ro")


class PathPlanningConfig(BaseModel):
    """Cấu hình path planning"""
    algorithm: str = Field("a_star", description="Thuật toán sử dụng")
    safety_margin: float = Field(100.0, description="Khoảng cách an toàn (mm)")
    max_route_length: float = Field(10000.0, description="Độ dài route tối đa (mm)")
    optimization_weight: float = Field(0.7, description="Trọng số tối ưu hóa (0-1)")
    safety_weight: float = Field(0.3, description="Trọng số an toàn (0-1)")
    enable_dynamic_planning: bool = Field(True, description="Bật lập kế hoạch động")
    enable_obstacle_avoidance: bool = Field(True, description="Bật tránh chướng ngại vật")
    enable_safety_integration: bool = Field(True, description="Bật tích hợp an toàn")
    update_frequency: float = Field(1.0, description="Tần suất cập nhật (Hz)")


class PathPlanningRequest(BaseModel):
    """Yêu cầu lập kế hoạch đường đi"""
    start_point: Point = Field(..., description="Điểm bắt đầu")
    end_point: Point = Field(..., description="Điểm kết thúc")
    waypoints: List[Point] = Field(default_factory=list, description="Điểm dừng")
    obstacles: List[Obstacle] = Field(default_factory=list, description="Chướng ngại vật")
    safety_zones: List[Dict[str, Any]] = Field(default_factory=list, description="Vùng an toàn")
    max_speed: float = Field(1000.0, description="Tốc độ tối đa (mm/s)")
    max_acceleration: float = Field(500.0, description="Gia tốc tối đa (mm/s²)")
    route_type: RouteType = Field(RouteType.OPTIMIZED, description="Loại route mong muốn")
    constraints: Dict[str, Any] = Field(default_factory=dict, description="Ràng buộc bổ sung")


class PathPlanningResult(BaseModel):
    """Kết quả lập kế hoạch đường đi"""
    success: bool = Field(..., description="Thành công hay không")
    route: Optional[Route] = Field(None, description="Route được tạo")
    safe_route: Optional[SafeRoute] = Field(None, description="Route an toàn")
    planning_time: float = Field(0.0, description="Thời gian lập kế hoạch (s)")
    iterations: int = Field(0, description="Số lần lặp thuật toán")
    warnings: List[str] = Field(default_factory=list, description="Cảnh báo")
    errors: List[str] = Field(default_factory=list, description="Lỗi")
    alternatives: List[Route] = Field(default_factory=list, description="Route thay thế")
    metadata: Dict[str, Any] = Field(default_factory=dict, description="Metadata bổ sung")


class OptimizationMetrics(BaseModel):
    """Metrics tối ưu hóa"""
    distance_improvement: float = Field(0.0, description="Cải thiện khoảng cách (%)")
    time_improvement: float = Field(0.0, description="Cải thiện thời gian (%)")
    safety_improvement: float = Field(0.0, description="Cải thiện an toàn (%)")
    energy_efficiency: float = Field(1.0, description="Hiệu quả năng lượng (0-1)")
    smoothness_score: float = Field(1.0, description="Điểm mượt mà (0-1)")
    complexity_score: float = Field(1.0, description="Điểm phức tạp (0-1)")


class PathPlanningEvent(BaseModel):
    """Sự kiện path planning"""
    id: UUID = Field(default_factory=uuid4, description="ID duy nhất")
    timestamp: datetime = Field(default_factory=datetime.utcnow, description="Thời gian xảy ra")
    event_type: str = Field(..., description="Loại sự kiện")
    description: str = Field(..., description="Mô tả sự kiện")
    route_id: Optional[UUID] = Field(None, description="ID route liên quan")
    metrics: Optional[OptimizationMetrics] = Field(None, description="Metrics")
    data: Dict[str, Any] = Field(default_factory=dict, description="Dữ liệu bổ sung")
    severity: str = Field("info", description="Mức độ nghiêm trọng")
