"""
RS485 Module Schemas for OHT-50 Backend
Schemas for RS485 module management and monitoring
"""

from datetime import datetime
from typing import Dict, Any, List, Optional
from pydantic import BaseModel, Field
from enum import Enum


class RS485ModuleType(str, Enum):
    """RS485 Module Types - Match with firmware modules.yaml"""
    UNKNOWN = "unknown"          # type: 0
    POWER = "power"              # type: 2 
    SAFETY = "safety"            # type: 3
    TRAVEL = "travel"            # type: 4
    DOCK_LOCATION = "dock_location"  # type: 5
    MASTER = "master"            # type: 6
    LIFTER = "lifter"            # type: 7
    CARGO = "cargo"              # type: 8


class RS485ModuleStatus(str, Enum):
    """RS485 Module Status"""
    HEALTHY = "healthy"
    WARNING = "warning"
    ERROR = "error"
    LOST = "lost"
    OFFLINE = "offline"


class RS485BusStatus(str, Enum):
    """RS485 Bus Status"""
    ONLINE = "online"
    WARNING = "warning"
    ERROR = "error"
    OFFLINE = "offline"


class RS485ModuleRealTime(BaseModel):
    """Real-time telemetry data from RS485 module"""
    battery: float = Field(..., description="Battery level (%)")
    voltage: float = Field(..., description="Voltage (V)")
    current: float = Field(..., description="Current (A)")
    temperature: float = Field(..., description="Temperature (°C)")
    timestamp: datetime = Field(default_factory=datetime.utcnow)


class RS485ModuleInfo(BaseModel):
    """RS485 Module Information - Match with firmware format"""
    address: int = Field(..., description="Module address (0x01-0x0F)", ge=1, le=15)
    addr: str = Field(..., description="Module address as hex string (0x01)")
    name: str = Field(..., description="Module name")
    type: RS485ModuleType = Field(..., description="Module type")
    status: RS485ModuleStatus = Field(..., description="Module status")
    last_seen: str = Field(..., description="Last seen time")
    last_seen_ms: int = Field(..., description="Last seen timestamp (ms)")
    error_rate: float = Field(..., description="Error rate (%)", ge=0, le=100)
    response_time: int = Field(..., description="Response time (ms)")
    fw_version: str = Field(..., description="Firmware version")
    mandatory: bool = Field(..., description="Is mandatory module")
    capabilities: int = Field(default=0, description="Module capabilities bitmask")
    version: str = Field(default="", description="Module version string")
    real_time: RS485ModuleRealTime = Field(..., description="Real-time data")


class RS485BusHealth(BaseModel):
    """RS485 Bus Health Information"""
    status: RS485BusStatus = Field(..., description="Bus status")
    error_rate: float = Field(..., description="Bus error rate (%)")
    response_time_p95: int = Field(..., description="95th percentile response time (ms)")
    throughput: int = Field(..., description="Bus throughput (fps)")
    last_scan: str = Field(..., description="Last scan time")
    total_modules: int = Field(..., description="Total modules found")
    active_modules: int = Field(..., description="Active modules")
    failed_modules: int = Field(..., description="Failed modules")


class RS485DiscoveryStatus(BaseModel):
    """RS485 Discovery Status"""
    is_running: bool = Field(..., description="Is discovery running")
    progress: int = Field(..., description="Discovery progress (%)", ge=0, le=100)
    status_message: str = Field(..., description="Status message")
    modules_found: int = Field(..., description="Number of modules found")
    conflicts: List[str] = Field(default_factory=list, description="Address conflicts")
    start_time: Optional[datetime] = Field(None, description="Discovery start time")
    end_time: Optional[datetime] = Field(None, description="Discovery end time")


class RS485DiscoveryResult(BaseModel):
    """RS485 Discovery Result"""
    address: int = Field(..., description="Module address")
    addr: str = Field(..., description="Address as hex string")
    name: str = Field(..., description="Module name")
    type: RS485ModuleType = Field(..., description="Module type")
    found: bool = Field(..., description="Module found")
    conflict: bool = Field(default=False, description="Address conflict")
    response_time: Optional[int] = Field(None, description="Response time (ms)")


# Request/Response Models
class RS485ModulesResponse(BaseModel):
    """Response for GET /api/v1/rs485/modules"""
    success: bool = Field(..., description="Request success")
    data: List[RS485ModuleInfo] = Field(..., description="List of RS485 modules")
    message: str = Field(..., description="Response message")
    timestamp: datetime = Field(default_factory=datetime.utcnow)


class RS485ModuleResponse(BaseModel):
    """Response for GET /api/v1/rs485/modules/{addr}"""
    success: bool = Field(..., description="Request success")
    data: RS485ModuleInfo = Field(..., description="RS485 module info")
    message: str = Field(..., description="Response message")
    timestamp: datetime = Field(default_factory=datetime.utcnow)


class RS485BusHealthResponse(BaseModel):
    """Response for GET /api/v1/rs485/bus/health"""
    success: bool = Field(..., description="Request success")
    data: RS485BusHealth = Field(..., description="RS485 bus health")
    message: str = Field(..., description="Response message")
    timestamp: datetime = Field(default_factory=datetime.utcnow)


class RS485DiscoveryResponse(BaseModel):
    """Response for RS485 discovery operations"""
    success: bool = Field(..., description="Request success")
    data: RS485DiscoveryStatus = Field(..., description="Discovery status")
    message: str = Field(..., description="Response message")
    timestamp: datetime = Field(default_factory=datetime.utcnow)


class RS485DiscoveryResultsResponse(BaseModel):
    """Response for GET /api/v1/rs485/discovery/results"""
    success: bool = Field(..., description="Request success")
    data: List[RS485DiscoveryResult] = Field(..., description="Discovery results")
    message: str = Field(..., description="Response message")
    timestamp: datetime = Field(default_factory=datetime.utcnow)


class RS485ModuleActionRequest(BaseModel):
    """Request for module actions (ping, reset, etc.)"""
    action: str = Field(..., description="Action to perform")
    parameters: Dict[str, Any] = Field(default_factory=dict, description="Action parameters")


class RS485ModuleActionResponse(BaseModel):
    """Response for module actions"""
    success: bool = Field(..., description="Action success")
    data: Dict[str, Any] = Field(..., description="Action result data")
    message: str = Field(..., description="Response message")
    timestamp: datetime = Field(default_factory=datetime.utcnow)


class RS485BusActionRequest(BaseModel):
    """Request for bus actions (restart, etc.)"""
    action: str = Field(..., description="Action to perform")
    force: bool = Field(default=False, description="Force action")


class RS485BusActionResponse(BaseModel):
    """Response for bus actions"""
    success: bool = Field(..., description="Action success")
    data: Dict[str, Any] = Field(..., description="Action result data")
    message: str = Field(..., description="Response message")
    timestamp: datetime = Field(default_factory=datetime.utcnow)
