"""
Pydantic models cho quản lý RS485: cấu hình bus, registry module, điểm đo
"""

from typing import Any, Dict, List, Optional

from pydantic import BaseModel, Field, ConfigDict


class BusAddresses(BaseModel):
    """Ánh xạ địa chỉ RS485 theo module"""

    model_config = ConfigDict()

    safety: Optional[int] = Field(default=None, description="Safety module address")
    dio_upper: Optional[int] = Field(default=None, description="DIO Upper address")
    aio_upper: Optional[int] = Field(default=None, description="AIO Upper address")
    dcm1: Optional[int] = Field(default=None, description="DC Motor Driver #1")
    stm1: Optional[int] = Field(default=None, description="Stepper Driver #1")
    dcm2: Optional[int] = Field(default=None, description="DC Motor Driver #2")
    stm2: Optional[int] = Field(default=None, description="Stepper Driver #2")
    dio_cargo: Optional[int] = Field(default=None, description="DIO Cargo address")
    aio_cargo: Optional[int] = Field(default=None, description="AIO Cargo address")
    dcm3: Optional[int] = Field(default=None, description="DC Motor Driver #3")
    location: Optional[int] = Field(default=None, description="Location (RFID+Encoder)")


class BusConfig(BaseModel):
    """Cấu hình RS485 tối thiểu theo config_spec"""

    model_config = ConfigDict()

    device: str = Field(description="Device path, ví dụ /dev/ttyOHT485")
    baud: int = Field(description="Baud rate, ví dụ 115200")
    addresses: Optional[BusAddresses] = Field(default=None, description="Ánh xạ địa chỉ")


class ModuleInfo(BaseModel):
    """Thông tin module trên bus"""

    model_config = ConfigDict()

    address: int = Field(description="Địa chỉ module")
    module_type: str = Field(description="Loại module, ví dụ safety/dio/aio/dcm/stepper/location")
    name: Optional[str] = Field(default=None, description="Tên thân thiện")
    online: bool = Field(default=False, description="Trạng thái online")
    last_seen_ts: Optional[float] = Field(default=None, description="Lần cuối thấy")
    meta: Dict[str, Any] = Field(default_factory=dict, description="Thông tin thêm")


class DigitalSnapshot(BaseModel):
    model_config = ConfigDict()

    di_upper: Optional[int] = Field(default=None, description="Bitmask DI upper")
    di_cargo: Optional[int] = Field(default=None, description="Bitmask DI cargo")


class AnalogSnapshot(BaseModel):
    model_config = ConfigDict()

    aio_upper: Optional[List[float]] = Field(default=None, description="Danh sách AI upper")
    aio_cargo: Optional[List[float]] = Field(default=None, description="Danh sách AI cargo")


class MotorFeedback(BaseModel):
    model_config = ConfigDict()

    address: int = Field(description="Địa chỉ driver")
    pos_mm: Optional[float] = Field(default=None, description="Vị trí")
    vel_mms: Optional[float] = Field(default=None, description="Vận tốc")
    current_a: Optional[float] = Field(default=None, description="Dòng (A)")
    temperature_c: Optional[float] = Field(default=None, description="Nhiệt độ (°C)")
    fault: Optional[str] = Field(default=None, description="Mã lỗi nếu có")


class PointsSnapshot(BaseModel):
    """Ảnh chụp điểm đo tổng hợp cho giám sát nhanh"""

    model_config = ConfigDict()

    digital: DigitalSnapshot = Field(default_factory=DigitalSnapshot)
    analog: AnalogSnapshot = Field(default_factory=AnalogSnapshot)
    motors: List[MotorFeedback] = Field(default_factory=list)
    encoder_count: Optional[int] = Field(default=None, description="Count encoder (location)")
    tag_id: Optional[str] = Field(default=None, description="RFID tag id nếu có")
    rssi: Optional[int] = Field(default=None, description="Cường độ tín hiệu RFID nếu có")


