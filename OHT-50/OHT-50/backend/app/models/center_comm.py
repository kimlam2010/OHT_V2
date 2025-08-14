"""
Center Communication Models
Dựa trên docs/specs/center_comm.md
"""

from datetime import datetime
from enum import Enum
from typing import Any, Dict, List, Optional

from pydantic import BaseModel, Field, validator


class MissionType(str, Enum):
    """Loại nhiệm vụ từ Center"""

    MOVE_TO = "move_to"
    DOCK = "dock"
    UNDOCK = "undock"
    EMERGENCY_STOP = "emergency_stop"
    STATUS_REQUEST = "status_request"
    CONFIG_UPDATE = "config_update"


class MissionStatus(str, Enum):
    """Trạng thái nhiệm vụ"""

    PENDING = "pending"
    IN_PROGRESS = "in_progress"
    COMPLETED = "completed"
    FAILED = "failed"
    CANCELLED = "cancelled"


class Position(BaseModel):
    """Vị trí 3D"""

    x: float = Field(..., description="Tọa độ X (mm)")
    y: float = Field(..., description="Tọa độ Y (mm)")
    z: float = Field(0.0, description="Tọa độ Z (mm)")
    theta: float = Field(0.0, description="Góc quay (rad)")


class Mission(BaseModel):
    """Nhiệm vụ từ Center"""

    mission_id: str = Field(..., description="ID nhiệm vụ")
    mission_type: MissionType = Field(..., description="Loại nhiệm vụ")
    target_position: Optional[Position] = Field(None, description="Vị trí đích")
    parameters: Dict[str, Any] = Field(
        default_factory=dict, description="Tham số bổ sung"
    )
    priority: int = Field(1, description="Độ ưu tiên (1-10)")
    timeout: Optional[int] = Field(None, description="Timeout (giây)")
    created_at: datetime = Field(
        default_factory=datetime.now, description="Thời gian tạo"
    )


class MissionResponse(BaseModel):
    """Phản hồi nhiệm vụ"""

    mission_id: str = Field(..., description="ID nhiệm vụ")
    status: MissionStatus = Field(..., description="Trạng thái nhiệm vụ")
    message: str = Field(..., description="Thông báo")
    progress: Optional[float] = Field(None, ge=0.0, le=100.0, description="Tiến độ (%)")
    estimated_completion: Optional[datetime] = Field(
        None, description="Thời gian hoàn thành dự kiến"
    )
    error_code: Optional[str] = Field(None, description="Mã lỗi nếu có")
    timestamp: datetime = Field(
        default_factory=datetime.now, description="Thời gian phản hồi"
    )


class PermissionRequest(BaseModel):
    """Yêu cầu quyền từ Center"""

    request_id: str = Field(..., description="ID yêu cầu")
    action: str = Field(..., description="Hành động cần quyền")
    parameters: Dict[str, Any] = Field(default_factory=dict, description="Tham số")
    urgency: str = Field("normal", description="Mức độ khẩn cấp")
    timestamp: datetime = Field(
        default_factory=datetime.now, description="Thời gian yêu cầu"
    )


class PermissionResponse(BaseModel):
    """Phản hồi quyền từ Center"""

    request_id: str = Field(..., description="ID yêu cầu")
    granted: bool = Field(..., description="Được cấp quyền")
    message: str = Field(..., description="Thông báo")
    conditions: Optional[Dict[str, Any]] = Field(None, description="Điều kiện bổ sung")
    valid_until: Optional[datetime] = Field(None, description="Hiệu lực đến")
    timestamp: datetime = Field(
        default_factory=datetime.now, description="Thời gian phản hồi"
    )


class StatusReport(BaseModel):
    """Báo cáo trạng thái gửi Center"""

    report_id: str = Field(..., description="ID báo cáo")
    device_id: str = Field(..., description="ID thiết bị")
    status: str = Field(..., description="Trạng thái hiện tại")
    position: Optional[Position] = Field(None, description="Vị trí hiện tại")
    battery_level: Optional[float] = Field(
        None, ge=0.0, le=100.0, description="Mức pin (%)"
    )
    error_codes: List[str] = Field(default_factory=list, description="Danh sách mã lỗi")
    warnings: List[str] = Field(default_factory=list, description="Danh sách cảnh báo")
    timestamp: datetime = Field(
        default_factory=datetime.now, description="Thời gian báo cáo"
    )


class CenterMessage(BaseModel):
    """Tin nhắn giao tiếp với Center"""

    message_id: str = Field(..., description="ID tin nhắn")
    message_type: str = Field(..., description="Loại tin nhắn")
    payload: Dict[str, Any] = Field(..., description="Nội dung tin nhắn")
    correlation_id: Optional[str] = Field(None, description="ID tương quan")
    timestamp: datetime = Field(
        default_factory=datetime.now, description="Thời gian gửi"
    )
    ttl: Optional[int] = Field(None, description="Thời gian sống (giây)")


class CommunicationConfig(BaseModel):
    """Cấu hình giao tiếp với Center"""

    center_url: str = Field(..., description="URL của Center")
    api_key: str = Field(..., description="API key")
    timeout: int = Field(30, description="Timeout kết nối (giây)")
    retry_attempts: int = Field(3, description="Số lần thử lại")
    heartbeat_interval: int = Field(60, description="Chu kỳ heartbeat (giây)")
    enable_ssl: bool = Field(True, description="Bật SSL")
    enable_compression: bool = Field(True, description="Bật nén dữ liệu")


class CommunicationStatus(BaseModel):
    """Trạng thái kết nối với Center"""

    connected: bool = Field(..., description="Đã kết nối")
    last_heartbeat: Optional[datetime] = Field(None, description="Heartbeat cuối")
    connection_quality: Optional[str] = Field(None, description="Chất lượng kết nối")
    error_count: int = Field(0, description="Số lỗi kết nối")
    last_error: Optional[str] = Field(None, description="Lỗi cuối cùng")
    uptime: Optional[int] = Field(None, description="Thời gian hoạt động (giây)")
