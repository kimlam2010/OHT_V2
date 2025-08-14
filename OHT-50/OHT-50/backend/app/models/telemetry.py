"""
Pydantic models cho Telemetry theo docs/specs/telemetry_schema.md
"""
from datetime import datetime
from typing import Any, Dict, List, Optional

from pydantic import BaseModel, Field


class TelemetryTarget(BaseModel):
    """Target values cho telemetry"""

    pos_mm: float = Field(description="Target position (mm)")
    vel_mms: float = Field(description="Target velocity (mm/s)")


class TelemetrySafety(BaseModel):
    """Safety status"""

    estop: bool = Field(default=False, description="E-Stop status")
    zone_blocked: bool = Field(default=False, description="Zone blocked status")
    interlock_active: bool = Field(default=False, description="Interlock active")


class TelemetryEncoder(BaseModel):
    """Encoder status"""

    health: str = Field(default="ok", description="Encoder health status")
    fault: Optional[str] = Field(default=None, description="Encoder fault message")
    count: int = Field(default=0, description="Encoder count")


class TelemetryStatus(BaseModel):
    """System status"""

    state: str = Field(description="Current state (Idle/Move/Dock/Fault/E-Stop)")
    pos_mm: float = Field(description="Current position (mm)")
    vel_mms: float = Field(description="Current velocity (mm/s)")
    acc_mms2: float = Field(description="Current acceleration (mm/s²)")
    target: TelemetryTarget = Field(description="Target values")
    safety: TelemetrySafety = Field(description="Safety status")
    enc: TelemetryEncoder = Field(description="Encoder status")
    errors: List[str] = Field(default_factory=list, description="Error messages")
    warn: List[str] = Field(default_factory=list, description="Warning messages")


class TelemetryData(BaseModel):
    """Complete telemetry data"""

    ts: float = Field(description="Timestamp (Unix timestamp)")
    status: TelemetryStatus = Field(description="System status")

    # Additional fields for extended telemetry
    mission_id: Optional[str] = Field(default=None, description="Current mission ID")
    station_id: Optional[str] = Field(default=None, description="Current station ID")
    cargo_present: bool = Field(default=False, description="Cargo present")
    cargo_locked: bool = Field(default=False, description="Cargo locked")

    # Performance metrics
    cpu_usage: Optional[float] = Field(default=None, description="CPU usage (%)")
    memory_usage: Optional[float] = Field(default=None, description="Memory usage (%)")
    temperature: Optional[float] = Field(default=None, description="Temperature (°C)")

    # Communication status
    center_connected: bool = Field(
        default=False, description="Center connection status"
    )
    rs485_connected: bool = Field(default=False, description="RS485 connection status")

    # Location (RFID) optional fields
    tag_id: Optional[str] = Field(default=None, description="Last RFID tag id (nếu có sự kiện)")
    rssi: Optional[int] = Field(default=None, description="RFID RSSI (nếu có)")

    class Config:
        json_encoders = {datetime: lambda v: v.isoformat()}


class TelemetryHistory(BaseModel):
    """Telemetry history response"""

    data: List[TelemetryData] = Field(description="Telemetry history")
    total: int = Field(description="Total records")
    page: int = Field(description="Current page")
    page_size: int = Field(description="Page size")
    has_more: bool = Field(description="Has more records")


class TelemetryFilter(BaseModel):
    """Filter for telemetry queries"""

    start_time: Optional[float] = Field(default=None, description="Start timestamp")
    end_time: Optional[float] = Field(default=None, description="End timestamp")
    state: Optional[str] = Field(default=None, description="Filter by state")
    has_errors: Optional[bool] = Field(default=None, description="Filter by errors")
    page: int = Field(default=1, description="Page number")
    page_size: int = Field(default=100, description="Page size")
