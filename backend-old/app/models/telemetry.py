"""
Pydantic models cho Telemetry theo docs/specs/telemetry_schema.md
"""
from datetime import datetime
from typing import Any, Dict, List, Optional

from pydantic import BaseModel, Field, ConfigDict


class TelemetryTarget(BaseModel):
    """Target values cho telemetry"""
    model_config = ConfigDict(
        json_schema_extra={
            "example": {
                "pos_mm": 150.0,
                "vel_mms": 50.0
            }
        }
    )

    pos_mm: float = Field(description="Target position (mm)")
    vel_mms: float = Field(description="Target velocity (mm/s)")


class TelemetrySafety(BaseModel):
    """Safety status"""
    model_config = ConfigDict(
        json_schema_extra={
            "example": {
                "estop": False,
                "zone_blocked": False,
                "interlock_active": False
            }
        }
    )

    estop: bool = Field(default=False, description="E-Stop status")
    zone_blocked: bool = Field(default=False, description="Zone blocked status")
    interlock_active: bool = Field(default=False, description="Interlock active")


class TelemetryEncoder(BaseModel):
    """Encoder status"""
    model_config = ConfigDict(
        json_schema_extra={
            "example": {
                "health": "ok",
                "fault": None,
                "count": 12345
            }
        }
    )

    health: str = Field(default="ok", description="Encoder health status")
    fault: Optional[str] = Field(default=None, description="Encoder fault message")
    count: int = Field(default=0, description="Encoder count")


class TelemetryStatus(BaseModel):
    """System status"""
    model_config = ConfigDict(
        json_schema_extra={
            "example": {
                "state": "Move",
                "pos_mm": 125.5,
                "vel_mms": 45.2,
                "acc_mms2": 12.8,
                "target": {
                    "pos_mm": 150.0,
                    "vel_mms": 50.0
                },
                "safety": {
                    "estop": False,
                    "zone_blocked": False,
                    "interlock_active": False
                },
                "enc": {
                    "health": "ok",
                    "fault": None,
                    "count": 12345
                },
                "errors": [],
                "warn": ["Low battery: 15%"]
            }
        }
    )

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
    model_config = ConfigDict(
        json_schema_extra={
            "example": {
                "ts": 1705312800.123,
                "status": {
                    "state": "Move",
                    "pos_mm": 125.5,
                    "vel_mms": 45.2,
                    "acc_mms2": 12.8,
                    "target": {
                        "pos_mm": 150.0,
                        "vel_mms": 50.0
                    },
                    "safety": {
                        "estop": False,
                        "zone_blocked": False,
                        "interlock_active": False
                    },
                    "enc": {
                        "health": "ok",
                        "fault": None,
                        "count": 12345
                    },
                    "errors": [],
                    "warn": ["Low battery: 15%"]
                },
                "mission_id": "m-001",
                "station_id": "s-005",
                "cargo_present": True,
                "cargo_locked": True,
                "cpu_usage": 45.2,
                "memory_usage": 67.8,
                "temperature": 42.5,
                "center_connected": True,
                "rs485_connected": True,
                "tag_id": "RFID-12345",
                "rssi": -45
            }
        }
    )

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

    # Note: json_encoders moved to model_config in Pydantic v2


class TelemetryHistory(BaseModel):
    """Telemetry history response"""
    model_config = ConfigDict(
        json_schema_extra={
            "example": {
                "data": [
                    {
                        "ts": 1705312800.123,
                        "status": {
                            "state": "Move",
                            "pos_mm": 125.5,
                            "vel_mms": 45.2,
                            "acc_mms2": 12.8,
                            "target": {"pos_mm": 150.0, "vel_mms": 50.0},
                            "safety": {"estop": False, "zone_blocked": False, "interlock_active": False},
                            "enc": {"health": "ok", "fault": None, "count": 12345},
                            "errors": [],
                            "warn": ["Low battery: 15%"]
                        }
                    }
                ],
                "total": 100,
                "page": 1,
                "page_size": 10,
                "has_more": True
            }
        }
    )

    data: List[TelemetryData] = Field(description="Telemetry history")
    total: int = Field(description="Total records")
    page: int = Field(description="Current page")
    page_size: int = Field(description="Page size")
    has_more: bool = Field(description="Has more records")


class TelemetryFilter(BaseModel):
    """Filter for telemetry queries"""
    model_config = ConfigDict(
        json_schema_extra={
            "example": {
                "start_time": 1705312800.0,
                "end_time": 1705316400.0,
                "state": "Move",
                "has_errors": False,
                "page": 1,
                "page_size": 100
            }
        }
    )

    start_time: Optional[float] = Field(default=None, description="Start timestamp")
    end_time: Optional[float] = Field(default=None, description="End timestamp")
    state: Optional[str] = Field(default=None, description="Filter by state")
    has_errors: Optional[bool] = Field(default=None, description="Filter by errors")
    page: int = Field(default=1, description="Page number")
    page_size: int = Field(default=100, description="Page size")


class LogRecord(BaseModel):
    """Định dạng một bản ghi log đơn giản"""
    model_config = ConfigDict(
        json_schema_extra={
            "example": {
                "time": "2024-01-15T14:45:00.123Z",
                "level": "INFO",
                "source": "config",
                "message": "Configuration updated successfully"
            }
        }
    )

    time: str = Field(description="Timestamp ISO8601")
    level: str = Field(description="Level")
    source: str = Field(description="Source")
    message: str = Field(description="Log message")


class AuditRecord(BaseModel):
    """Định dạng một bản ghi audit trail"""
    model_config = ConfigDict(
        json_schema_extra={
            "example": {
                "time": "2024-01-15T14:45:00.123Z",
                "user": "alice",
                "action": "config_update",
                "target": "system_config",
                "result": "OK",
                "ip": "192.168.1.100",
                "session": "S-12345",
                "device": "OHT-50-01",
                "correlation_id": "c-67890",
                "signature": "sha256:abc123def456"
            }
        }
    )

    time: str = Field(description="Timestamp ISO8601")
    user: str = Field(description="Username")
    action: str = Field(description="Action name")
    target: str = Field(default="-", description="Target entity")
    result: str = Field(default="OK", description="Result")
    ip: str = Field(default="127.0.0.1", description="Client IP")
    session: str = Field(default="S-1", description="Session ID")
    device: str = Field(default="OHT-50-01", description="Device ID")
    correlation_id: str = Field(default="c-0000", description="Correlation ID")
    signature: str = Field(default="sha256:mock", description="Signature")