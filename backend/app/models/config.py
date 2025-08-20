"""
Pydantic models cho Configuration theo docs/specs/config_spec.md
"""
from datetime import datetime
from typing import Any, Dict, List, Optional

from pydantic import BaseModel, Field, field_validator, ConfigDict


class MotorConfig(BaseModel):
    """Motor configuration"""
    model_config = ConfigDict()

    interface: str = Field(default="rs485", description="Interface type (rs485|can)")
    r: float = Field(description="Bán kính puly (m)")
    vmax_mms: float = Field(description="Tốc độ tối đa (mm/s)")
    amax_mms2: float = Field(description="Gia tốc tối đa (mm/s²)")
    jmax_mms3: float = Field(description="Jerk tối đa (mm/s³)")

    @field_validator("r", "vmax_mms", "amax_mms2", "jmax_mms3")
    @classmethod
    def validate_positive(cls, v):
        if v <= 0:
            raise ValueError("Giá trị phải lớn hơn 0")
        return v

    @field_validator("interface")
    @classmethod
    def validate_interface(cls, v):
        if v not in ["rs485", "can"]:
            raise ValueError("Interface phải là rs485 hoặc can")
        return v


class EncoderConfig(BaseModel):
    """Encoder configuration"""
    model_config = ConfigDict()

    type: str = Field(default="incremental", description="Encoder type")
    resolution_ppr: int = Field(description="Resolution (pulses per revolution)")
    invert: bool = Field(default=False, description="Invert encoder direction")

    @field_validator("resolution_ppr")
    @classmethod
    def validate_resolution(cls, v):
        if v <= 0:
            raise ValueError("Resolution phải lớn hơn 0")
        return v


class SafetyZone(BaseModel):
    """Safety zone configuration"""
    model_config = ConfigDict()

    start_mm: float = Field(description="Start position (mm)")
    end_mm: float = Field(description="End position (mm)")
    vmax_mms: float = Field(description="Tốc độ tối đa trong zone (mm/s)")

    @field_validator("end_mm")
    @classmethod
    def validate_end_position(cls, v, info):
        if "start_mm" in info.data and v <= info.data["start_mm"]:
            raise ValueError("End position phải lớn hơn start position")
        return v


class SafetyConfig(BaseModel):
    """Safety configuration"""
    model_config = ConfigDict()

    zones: List[SafetyZone] = Field(default_factory=list, description="Safety zones")
    estop_reset_policy: str = Field(default="manual", description="E-Stop reset policy")

    @field_validator("estop_reset_policy")
    @classmethod
    def validate_reset_policy(cls, v):
        if v not in ["manual", "auto"]:
            raise ValueError("Reset policy phải là manual hoặc auto")
        return v


class TelemetryConfig(BaseModel):
    """Telemetry configuration"""
    model_config = ConfigDict()

    ws_hz: int = Field(default=10, description="WebSocket rate (Hz)")
    history_size: int = Field(default=1000, description="History buffer size")

    @field_validator("ws_hz")
    @classmethod
    def validate_ws_rate(cls, v):
        if v < 1 or v > 50:
            raise ValueError("WebSocket rate phải từ 1-50 Hz")
        return v


class RS485Config(BaseModel):
    """RS485 communication configuration"""
    model_config = ConfigDict()

    device: str = Field(default="/dev/ttyOHT485", description="Device path")
    baud: int = Field(default=115200, description="Baud rate")
    timeout: float = Field(default=1.0, description="Timeout (seconds)")

    @field_validator("baud")
    @classmethod
    def validate_baud(cls, v):
        valid_bauds = [9600, 19200, 38400, 57600, 115200, 230400, 460800, 921600]
        if v not in valid_bauds:
            raise ValueError(f"Baud rate phải là một trong: {valid_bauds}")
        return v


class CANConfig(BaseModel):
    """CAN communication configuration"""
    model_config = ConfigDict()

    iface: str = Field(default="can0", description="CAN interface")
    bitrate: int = Field(default=500000, description="Bitrate")

    @field_validator("bitrate")
    @classmethod
    def validate_bitrate(cls, v):
        valid_bitrates = [125000, 250000, 500000, 1000000]
        if v not in valid_bitrates:
            raise ValueError(f"Bitrate phải là một trong: {valid_bitrates}")
        return v


class CommunicationConfig(BaseModel):
    """Communication configuration"""
    model_config = ConfigDict()

    rs485: Optional[RS485Config] = Field(default=None, description="RS485 config")
    can: Optional[CANConfig] = Field(default=None, description="CAN config")

    @field_validator("rs485", "can")
    @classmethod
    def validate_interface_config(cls, v, info):
        # Nếu có motor interface rs485 thì phải có rs485 config
        if "hardware" in info.data and info.data["hardware"].motor.interface == "rs485":
            if not info.data.get("rs485"):
                raise ValueError("RS485 config required when motor interface is rs485")
        return v


class HardwareConfig(BaseModel):
    """Hardware configuration"""
    model_config = ConfigDict()

    motor: MotorConfig = Field(description="Motor configuration")
    encoder: EncoderConfig = Field(description="Encoder configuration")


class SystemConfig(BaseModel):
    """Complete system configuration"""
    model_config = ConfigDict(
        json_schema_extra={
            "example": {
                "version": 1,
                "hardware": {
                    "motor": {
                        "interface": "rs485",
                        "r": 0.05,
                        "vmax_mms": 500.0,
                        "amax_mms2": 300.0,
                        "jmax_mms3": 5000.0,
                    },
                    "encoder": {
                        "type": "incremental",
                        "resolution_ppr": 2048,
                        "invert": False,
                    },
                },
                "safety": {
                    "zones": [{"start_mm": 0, "end_mm": 200, "vmax_mms": 200}],
                    "estop_reset_policy": "manual",
                },
                "telemetry": {"ws_hz": 10},
                "comm": {"rs485": {"device": "/dev/ttyOHT485", "baud": 115200}},
            }
        }
    )

    version: int = Field(default=1, description="Configuration version")
    hardware: Dict[str, Any] = Field(description="Hardware configuration")
    safety: Dict[str, Any] = Field(description="Safety configuration")
    telemetry: Dict[str, Any] = Field(description="Telemetry configuration")
    comm: Dict[str, Any] = Field(description="Communication configuration")

    # Metadata
    created_at: Optional[str] = Field(default=None, description="Creation timestamp")
    updated_at: Optional[str] = Field(default=None, description="Last update timestamp")
    description: Optional[str] = Field(
        default=None, description="Configuration description"
    )


class ConfigHistory(BaseModel):
    """Configuration history entry"""
    model_config = ConfigDict()

    version: int = Field(description="Configuration version")
    timestamp: datetime = Field(description="Timestamp")
    action: str = Field(description="Action performed")
    user: str = Field(description="User who made the change")
    config_data: Dict[str, Any] = Field(description="Configuration data")


class ConfigResponse(BaseModel):
    """Configuration API response"""
    model_config = ConfigDict()

    success: bool = Field(description="Operation success")
    message: str = Field(description="Response message")
    config: Optional[SystemConfig] = Field(default=None, description="Configuration data")
    version: Optional[int] = Field(default=None, description="Configuration version")
