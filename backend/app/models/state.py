from datetime import datetime
from enum import Enum
from typing import List, Optional, Dict, Any
from pydantic import BaseModel, Field
from uuid import UUID, uuid4


class SystemState(str, Enum):
    """Trạng thái hệ thống"""
    INITIALIZING = "initializing"
    IDLE = "idle"
    MOVING = "moving"
    DOCKING = "docking"
    DOCKED = "docked"
    FAULT = "fault"
    EMERGENCY_STOP = "emergency_stop"
    MAINTENANCE = "maintenance"
    SHUTDOWN = "shutdown"


class SubsystemState(str, Enum):
    """Trạng thái subsystem"""
    ONLINE = "online"
    OFFLINE = "offline"
    ERROR = "error"
    WARNING = "warning"
    MAINTENANCE = "maintenance"


class StateTransition(BaseModel):
    """Chuyển đổi trạng thái"""
    id: UUID = Field(default_factory=uuid4, description="ID duy nhất")
    from_state: SystemState = Field(..., description="Trạng thái từ")
    to_state: SystemState = Field(..., description="Trạng thái đến")
    timestamp: datetime = Field(default_factory=datetime.utcnow, description="Thời gian chuyển đổi")
    trigger: str = Field(..., description="Nguyên nhân chuyển đổi")
    user: Optional[str] = Field(None, description="Người dùng thực hiện")
    metadata: Dict[str, Any] = Field(default_factory=dict, description="Metadata bổ sung")
    is_valid: bool = Field(True, description="Chuyển đổi có hợp lệ không")
    error_message: Optional[str] = Field(None, description="Thông báo lỗi nếu có")


class SubsystemStatus(BaseModel):
    """Trạng thái subsystem"""
    subsystem_id: str = Field(..., description="ID của subsystem")
    name: str = Field(..., description="Tên subsystem")
    state: SubsystemState = Field(..., description="Trạng thái hiện tại")
    health: float = Field(1.0, description="Sức khỏe (0-1)")
    last_heartbeat: datetime = Field(default_factory=datetime.utcnow, description="Thời gian heartbeat cuối")
    error_count: int = Field(0, description="Số lỗi")
    warning_count: int = Field(0, description="Số cảnh báo")
    uptime: float = Field(0.0, description="Thời gian hoạt động (s)")
    version: Optional[str] = Field(None, description="Phiên bản")
    metadata: Dict[str, Any] = Field(default_factory=dict, description="Metadata bổ sung")


class SystemStatus(BaseModel):
    """Trạng thái tổng thể hệ thống"""
    system_state: SystemState = Field(SystemState.INITIALIZING, description="Trạng thái hệ thống")
    subsystems: Dict[str, SubsystemStatus] = Field(default_factory=dict, description="Trạng thái các subsystem")
    overall_health: float = Field(1.0, description="Sức khỏe tổng thể (0-1)")
    uptime: float = Field(0.0, description="Thời gian hoạt động (s)")
    last_state_change: datetime = Field(default_factory=datetime.utcnow, description="Thời gian thay đổi trạng thái cuối")
    active_alerts: int = Field(0, description="Số cảnh báo đang hoạt động")
    active_errors: int = Field(0, description="Số lỗi đang hoạt động")
    current_mission: Optional[str] = Field(None, description="Mission hiện tại")
    battery_level: Optional[float] = Field(None, description="Mức pin (%)")
    temperature: Optional[float] = Field(None, description="Nhiệt độ (°C)")
    metadata: Dict[str, Any] = Field(default_factory=dict, description="Metadata bổ sung")


class StateHistory(BaseModel):
    """Lịch sử trạng thái"""
    id: UUID = Field(default_factory=uuid4, description="ID duy nhất")
    system_state: SystemState = Field(..., description="Trạng thái hệ thống")
    timestamp: datetime = Field(default_factory=datetime.utcnow, description="Thời gian")
    duration: Optional[float] = Field(None, description="Thời gian duy trì (s)")
    trigger: Optional[str] = Field(None, description="Nguyên nhân")
    user: Optional[str] = Field(None, description="Người dùng")
    metadata: Dict[str, Any] = Field(default_factory=dict, description="Metadata bổ sung")


class StateValidation(BaseModel):
    """Validation trạng thái"""
    is_valid: bool = Field(..., description="Trạng thái có hợp lệ không")
    errors: List[str] = Field(default_factory=list, description="Danh sách lỗi")
    warnings: List[str] = Field(default_factory=list, description="Danh sách cảnh báo")
    required_conditions: List[str] = Field(default_factory=list, description="Điều kiện bắt buộc")
    optional_conditions: List[str] = Field(default_factory=list, description="Điều kiện tùy chọn")


class StateConfig(BaseModel):
    """Cấu hình quản lý trạng thái"""
    auto_save_interval: float = Field(30.0, description="Chu kỳ lưu tự động (s)")
    max_history_size: int = Field(1000, description="Kích thước lịch sử tối đa")
    heartbeat_timeout: float = Field(5.0, description="Timeout heartbeat (s)")
    state_transition_timeout: float = Field(10.0, description="Timeout chuyển đổi trạng thái (s)")
    validation_enabled: bool = Field(True, description="Bật validation")
    logging_enabled: bool = Field(True, description="Bật logging")
    backup_enabled: bool = Field(True, description="Bật backup")


class StateEvent(BaseModel):
    """Sự kiện trạng thái"""
    id: UUID = Field(default_factory=uuid4, description="ID duy nhất")
    timestamp: datetime = Field(default_factory=datetime.utcnow, description="Thời gian xảy ra")
    event_type: str = Field(..., description="Loại sự kiện")
    description: str = Field(..., description="Mô tả sự kiện")
    from_state: Optional[SystemState] = Field(None, description="Trạng thái từ")
    to_state: Optional[SystemState] = Field(None, description="Trạng thái đến")
    subsystem_id: Optional[str] = Field(None, description="ID subsystem")
    severity: str = Field("info", description="Mức độ nghiêm trọng")
    data: Dict[str, Any] = Field(default_factory=dict, description="Dữ liệu bổ sung")


class StateTransitionRule(BaseModel):
    """Quy tắc chuyển đổi trạng thái"""
    id: UUID = Field(default_factory=uuid4, description="ID duy nhất")
    from_state: SystemState = Field(..., description="Trạng thái từ")
    to_state: SystemState = Field(..., description="Trạng thái đến")
    trigger: str = Field(..., description="Điều kiện kích hoạt")
    conditions: List[str] = Field(default_factory=list, description="Điều kiện bắt buộc")
    actions: List[str] = Field(default_factory=list, description="Hành động thực hiện")
    timeout: Optional[float] = Field(None, description="Timeout (s)")
    is_enabled: bool = Field(True, description="Quy tắc có đang hoạt động không")
    priority: int = Field(0, description="Độ ưu tiên")
    created_at: datetime = Field(default_factory=datetime.utcnow, description="Thời gian tạo")


class StateSnapshot(BaseModel):
    """Snapshot trạng thái"""
    id: UUID = Field(default_factory=uuid4, description="ID duy nhất")
    timestamp: datetime = Field(default_factory=datetime.utcnow, description="Thời gian tạo")
    system_status: SystemStatus = Field(..., description="Trạng thái hệ thống")
    state_history: List[StateHistory] = Field(default_factory=list, description="Lịch sử trạng thái")
    events: List[StateEvent] = Field(default_factory=list, description="Sự kiện")
    metadata: Dict[str, Any] = Field(default_factory=dict, description="Metadata bổ sung")
    checksum: Optional[str] = Field(None, description="Checksum để verify")
