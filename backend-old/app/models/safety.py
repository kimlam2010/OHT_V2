from datetime import datetime
from enum import Enum
from typing import List, Optional, Dict, Any
from pydantic import BaseModel, Field
from uuid import UUID, uuid4


class SafetyZoneType(str, Enum):
    """Loại vùng an toàn"""
    RESTRICTED = "restricted"  # Vùng cấm
    WARNING = "warning"        # Vùng cảnh báo
    SLOW = "slow"             # Vùng giảm tốc
    EMERGENCY = "emergency"    # Vùng khẩn cấp


class EmergencyLevel(str, Enum):
    """Mức độ khẩn cấp"""
    LOW = "low"
    MEDIUM = "medium"
    HIGH = "high"
    CRITICAL = "critical"


class SafetyZone(BaseModel):
    """Vùng an toàn"""
    id: UUID = Field(default_factory=uuid4, description="ID duy nhất của vùng")
    name: str = Field(..., description="Tên vùng an toàn")
    zone_type: SafetyZoneType = Field(..., description="Loại vùng")
    center_x: float = Field(..., description="Tọa độ X trung tâm (mm)")
    center_y: float = Field(..., description="Tọa độ Y trung tâm (mm)")
    radius: float = Field(..., description="Bán kính vùng (mm)")
    max_speed: Optional[float] = Field(None, description="Tốc độ tối đa trong vùng (mm/s)")
    is_active: bool = Field(True, description="Vùng có đang hoạt động không")
    created_at: datetime = Field(default_factory=datetime.utcnow, description="Thời gian tạo")


class CollisionAlert(BaseModel):
    """Cảnh báo va chạm"""
    id: UUID = Field(default_factory=uuid4, description="ID duy nhất của cảnh báo")
    timestamp: datetime = Field(default_factory=datetime.utcnow, description="Thời gian phát hiện")
    alert_type: str = Field(..., description="Loại cảnh báo")
    severity: EmergencyLevel = Field(..., description="Mức độ nghiêm trọng")
    position_x: float = Field(..., description="Vị trí X (mm)")
    position_y: float = Field(..., description="Vị trí Y (mm)")
    distance: float = Field(..., description="Khoảng cách đến vật cản (mm)")
    object_type: Optional[str] = Field(None, description="Loại vật cản")
    confidence: float = Field(..., description="Độ tin cậy (0-1)")
    is_resolved: bool = Field(False, description="Đã được xử lý chưa")
    resolution_time: Optional[datetime] = Field(None, description="Thời gian xử lý")


class EmergencyStatus(BaseModel):
    """Trạng thái khẩn cấp"""
    is_emergency: bool = Field(False, description="Có đang trong trạng thái khẩn cấp không")
    emergency_level: EmergencyLevel = Field(EmergencyLevel.LOW, description="Mức độ khẩn cấp")
    emergency_type: Optional[str] = Field(None, description="Loại khẩn cấp")
    timestamp: datetime = Field(default_factory=datetime.utcnow, description="Thời gian bắt đầu")
    description: Optional[str] = Field(None, description="Mô tả tình huống")
    actions_taken: List[str] = Field(default_factory=list, description="Các hành động đã thực hiện")
    is_resolved: bool = Field(False, description="Đã được giải quyết chưa")
    resolution_time: Optional[datetime] = Field(None, description="Thời gian giải quyết")


class ZoneStatus(BaseModel):
    """Trạng thái vùng an toàn"""
    zone_id: UUID = Field(..., description="ID của vùng")
    zone_name: str = Field(..., description="Tên vùng")
    zone_type: SafetyZoneType = Field(..., description="Loại vùng")
    is_active: bool = Field(..., description="Vùng có đang hoạt động không")
    current_occupancy: int = Field(0, description="Số lượng vật thể trong vùng")
    last_scan_time: datetime = Field(default_factory=datetime.utcnow, description="Thời gian quét cuối")
    alerts: List[CollisionAlert] = Field(default_factory=list, description="Danh sách cảnh báo")


class SafetyStatus(BaseModel):
    """Trạng thái tổng thể của hệ thống an toàn"""
    system_status: str = Field("normal", description="Trạng thái hệ thống")
    emergency_status: EmergencyStatus = Field(..., description="Trạng thái khẩn cấp")
    active_zones: List[ZoneStatus] = Field(default_factory=list, description="Danh sách vùng đang hoạt động")
    recent_alerts: List[CollisionAlert] = Field(default_factory=list, description="Cảnh báo gần đây")
    estop_status: bool = Field(False, description="Trạng thái E-Stop")
    safety_system_health: float = Field(1.0, description="Sức khỏe hệ thống an toàn (0-1)")
    last_update: datetime = Field(default_factory=datetime.utcnow, description="Thời gian cập nhật cuối")


class SafetyConfig(BaseModel):
    """Cấu hình hệ thống an toàn"""
    estop_timeout: float = Field(0.05, description="Thời gian timeout E-Stop (s)")
    collision_detection_enabled: bool = Field(True, description="Bật phát hiện va chạm")
    safety_zone_monitoring: bool = Field(True, description="Bật giám sát vùng an toàn")
    emergency_procedure_timeout: float = Field(5.0, description="Timeout thủ tục khẩn cấp (s)")
    max_safe_speed: float = Field(1000.0, description="Tốc độ an toàn tối đa (mm/s)")
    min_safe_distance: float = Field(100.0, description="Khoảng cách an toàn tối thiểu (mm)")
    alert_retention_hours: int = Field(24, description="Thời gian lưu cảnh báo (giờ)")


class EmergencyProcedure(BaseModel):
    """Thủ tục khẩn cấp"""
    id: UUID = Field(default_factory=uuid4, description="ID duy nhất")
    name: str = Field(..., description="Tên thủ tục")
    description: str = Field(..., description="Mô tả thủ tục")
    emergency_level: EmergencyLevel = Field(..., description="Mức độ khẩn cấp")
    steps: List[str] = Field(..., description="Các bước thực hiện")
    timeout: float = Field(..., description="Thời gian timeout (s)")
    is_active: bool = Field(True, description="Thủ tục có đang hoạt động không")
    created_at: datetime = Field(default_factory=datetime.utcnow, description="Thời gian tạo")


class SafetyEvent(BaseModel):
    """Sự kiện an toàn"""
    id: UUID = Field(default_factory=uuid4, description="ID duy nhất")
    timestamp: datetime = Field(default_factory=datetime.utcnow, description="Thời gian xảy ra")
    event_type: str = Field(..., description="Loại sự kiện")
    severity: EmergencyLevel = Field(..., description="Mức độ nghiêm trọng")
    description: str = Field(..., description="Mô tả sự kiện")
    source: str = Field(..., description="Nguồn sự kiện")
    data: Dict[str, Any] = Field(default_factory=dict, description="Dữ liệu bổ sung")
    is_acknowledged: bool = Field(False, description="Đã được xác nhận chưa")
    acknowledged_by: Optional[str] = Field(None, description="Người xác nhận")
    acknowledged_at: Optional[datetime] = Field(None, description="Thời gian xác nhận")
