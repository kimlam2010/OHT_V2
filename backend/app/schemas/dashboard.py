"""
Dashboard schemas for OHT-50 Backend
"""

from pydantic import BaseModel, Field
from typing import List, Dict, Any, Optional
from datetime import datetime
from enum import Enum


class AlertSeverity(str, Enum):
    CRITICAL = "critical"
    HIGH = "high"
    MEDIUM = "medium"
    LOW = "low"
    INFO = "info"


class AlertStatus(str, Enum):
    ACTIVE = "active"
    ACKNOWLEDGED = "acknowledged"
    RESOLVED = "resolved"
    SUPPRESSED = "suppressed"


class LogLevel(str, Enum):
    DEBUG = "debug"
    INFO = "info"
    WARNING = "warning"
    ERROR = "error"
    CRITICAL = "critical"


class RobotMode(str, Enum):
    AUTO = "auto"
    MANUAL = "manual"
    SEMI = "semi"
    MAINTENANCE = "maintenance"
    EMERGENCY = "emergency"


class RobotStatus(str, Enum):
    IDLE = "idle"
    MOVING = "moving"
    DOCKING = "docking"
    CHARGING = "charging"
    ERROR = "error"
    EMERGENCY_STOP = "emergency_stop"
    PAUSED = "paused"


class AlertResponse(BaseModel):
    id: int = Field(..., description="Alert ID")
    title: str = Field(..., description="Alert title")
    message: str = Field(..., description="Alert message")
    severity: AlertSeverity = Field(..., description="Alert severity")
    status: AlertStatus = Field(..., description="Alert status")
    source: str = Field(..., description="Alert source")
    timestamp: datetime = Field(..., description="Alert timestamp")
    acknowledged_by: Optional[str] = Field(None, description="User who acknowledged")
    acknowledged_at: Optional[datetime] = Field(None, description="Acknowledgment timestamp")
    resolved_at: Optional[datetime] = Field(None, description="Resolution timestamp")
    metadata: Dict[str, Any] = Field(default_factory=dict, description="Additional alert data")


class LogResponse(BaseModel):
    id: int = Field(..., description="Log entry ID")
    level: LogLevel = Field(..., description="Log level")
    message: str = Field(..., description="Log message")
    source: str = Field(..., description="Log source")
    timestamp: datetime = Field(..., description="Log timestamp")
    user_id: Optional[int] = Field(None, description="User ID if applicable")
    session_id: Optional[str] = Field(None, description="Session ID if applicable")
    metadata: Dict[str, Any] = Field(default_factory=dict, description="Additional log data")


class PerformanceMetric(BaseModel):
    timestamp: datetime = Field(..., description="Metric timestamp")
    cpu_usage: float = Field(..., description="CPU usage percentage")
    memory_usage: float = Field(..., description="Memory usage percentage")
    disk_usage: float = Field(..., description="Disk usage percentage")
    network_in: float = Field(..., description="Network input bytes")
    network_out: float = Field(..., description="Network output bytes")
    response_time: float = Field(..., description="Average response time in ms")
    throughput: float = Field(..., description="Requests per second")


class RobotSummary(BaseModel):
    robot_id: str = Field(..., description="Robot ID")
    status: RobotStatus = Field(..., description="Current robot status")
    mode: RobotMode = Field(..., description="Current robot mode")
    position: Dict[str, float] = Field(..., description="Current position")
    battery_level: int = Field(..., description="Battery level percentage")
    temperature: float = Field(..., description="Robot temperature")
    speed: float = Field(..., description="Current speed")
    last_command: Optional[str] = Field(None, description="Last executed command")
    uptime: int = Field(..., description="Robot uptime in seconds")
    health_score: float = Field(..., description="Robot health score (0-100)")


class SystemSummary(BaseModel):
    total_alerts: int = Field(..., description="Total number of alerts")
    active_alerts: int = Field(..., description="Number of active alerts")
    critical_alerts: int = Field(..., description="Number of critical alerts")
    system_uptime: int = Field(..., description="System uptime in seconds")
    total_users: int = Field(..., description="Total number of users")
    active_users: int = Field(..., description="Number of active users")
    last_backup: Optional[datetime] = Field(None, description="Last backup timestamp")
    database_size: float = Field(..., description="Database size in MB")


class DashboardSummary(BaseModel):
    robot: RobotSummary = Field(..., description="Robot summary")
    system: SystemSummary = Field(..., description="System summary")
    performance: PerformanceMetric = Field(..., description="Current performance metrics")
    last_updated: datetime = Field(..., description="Last update timestamp")


class PerformanceDataResponse(BaseModel):
    metrics: List[PerformanceMetric] = Field(..., description="Performance metrics")
    time_range: str = Field(..., description="Time range of data")
    interval: str = Field(..., description="Data collection interval")
    total_points: int = Field(..., description="Total number of data points")


class AlertListResponse(BaseModel):
    alerts: List[AlertResponse] = Field(..., description="List of alerts")
    total_count: int = Field(..., description="Total number of alerts")
    page: int = Field(..., description="Current page number")
    page_size: int = Field(..., description="Number of items per page")
    has_next: bool = Field(..., description="Whether there are more pages")


class LogListResponse(BaseModel):
    logs: List[LogResponse] = Field(..., description="List of log entries")
    total_count: int = Field(..., description="Total number of log entries")
    page: int = Field(..., description="Current page number")
    page_size: int = Field(..., description="Number of items per page")
    has_next: bool = Field(..., description="Whether there are more pages")


class DashboardStats(BaseModel):
    total_requests: int = Field(..., description="Total API requests")
    successful_requests: int = Field(..., description="Successful requests")
    failed_requests: int = Field(..., description="Failed requests")
    average_response_time: float = Field(..., description="Average response time in ms")
    error_rate: float = Field(..., description="Error rate percentage")
    active_connections: int = Field(..., description="Active WebSocket connections")
    last_24h_requests: int = Field(..., description="Requests in last 24 hours")
