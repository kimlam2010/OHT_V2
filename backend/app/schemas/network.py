"""
Network schemas for OHT-50 Backend

Pydantic schemas for network configuration, status, and management.
"""

from pydantic import BaseModel, Field, field_validator, ValidationInfo
from typing import Dict, Any, Optional, List
from datetime import datetime
from enum import Enum


class NetworkStatus(str, Enum):
    """Network status enumeration"""
    CONNECTED = "connected"
    DISCONNECTED = "disconnected"
    CONNECTING = "connecting"
    ERROR = "error"
    AP_MODE = "ap_mode"
    FALLBACK = "fallback"


class WiFiAPStatus(str, Enum):
    """WiFi AP status enumeration"""
    STOPPED = "stopped"
    STARTING = "starting"
    RUNNING = "running"
    STOPPING = "stopping"
    ERROR = "error"


class NetworkInterface(str, Enum):
    """Network interface enumeration"""
    ETHERNET = "ethernet"
    WIFI = "wifi"
    AP = "ap"


class SecurityType(str, Enum):
    """WiFi security type enumeration"""
    OPEN = "Open"
    WPA2 = "WPA2"
    WPA3 = "WPA3"
    WPA2_WPA3 = "WPA2/WPA3"


class EventLevel(str, Enum):
    """Event level enumeration"""
    INFO = "info"
    WARNING = "warning"
    ERROR = "error"
    CRITICAL = "critical"


# Network Configuration Schemas

class WiFiAPConfig(BaseModel):
    """WiFi AP configuration schema"""
    ssid: str = Field(..., min_length=1, max_length=32, description="WiFi AP SSID")
    password: str = Field(..., min_length=8, max_length=63, description="WiFi AP password")
    channel: int = Field(..., ge=1, le=14, description="WiFi channel (1-14)")
    security: SecurityType = Field(default=SecurityType.WPA2, description="Security type")
    hidden: bool = Field(default=False, description="Hidden network")
    max_clients: int = Field(default=10, ge=1, le=50, description="Maximum clients")
    bandwidth_limit: Optional[int] = Field(None, ge=1, le=1000, description="Bandwidth limit in Mbps")
    
    @field_validator('ssid')
    @classmethod
    def validate_ssid(cls, v: str) -> str:
        if not v.strip():
            raise ValueError('SSID cannot be empty')
        return v.strip()
    
    @field_validator('password')
    @classmethod
    def validate_password(cls, v: str) -> str:
        if len(v) < 8:
            raise ValueError('Password must be at least 8 characters long')
        return v


class NetworkConfigRequest(BaseModel):
    """Network configuration request schema"""
    config_name: str = Field(..., min_length=1, max_length=100, description="Configuration name")
    config_type: str = Field(..., description="Configuration type")
    wifi_ap: Optional[WiFiAPConfig] = Field(None, description="WiFi AP configuration")
    ip_address: Optional[str] = Field(None, description="IP address")
    subnet_mask: Optional[str] = Field(None, description="Subnet mask")
    gateway: Optional[str] = Field(None, description="Gateway")
    dns_servers: Optional[List[str]] = Field(None, description="DNS servers")
    auto_start: bool = Field(default=False, description="Auto start configuration")
    
    @field_validator('config_type')
    @classmethod
    def validate_config_type(cls, v: str) -> str:
        valid_types = ['wifi_ap', 'ethernet', 'fallback']
        if v not in valid_types:
            raise ValueError(f'Config type must be one of: {", ".join(valid_types)}')
        return v


class NetworkConfigResponse(BaseModel):
    """Network configuration response schema"""
    success: bool = Field(..., description="Operation success status")
    message: str = Field(..., description="Response message")
    config_id: Optional[int] = Field(None, description="Configuration ID")
    timestamp: datetime = Field(..., description="Response timestamp")


# Network Status Schemas

class NetworkStatusResponse(BaseModel):
    """Network status response schema"""
    success: bool = Field(..., description="Operation success status")
    data: Dict[str, Any] = Field(..., description="Network status data")
    timestamp: datetime = Field(..., description="Response timestamp")


class NetworkStatusData(BaseModel):
    """Network status data schema"""
    status: NetworkStatus = Field(..., description="Network status")
    ap_status: Optional[WiFiAPStatus] = Field(None, description="WiFi AP status")
    fallback_enabled: bool = Field(..., description="Fallback connectivity enabled")
    primary_interface: Optional[NetworkInterface] = Field(None, description="Primary network interface")
    ip_address: Optional[str] = Field(None, description="IP address")
    mac_address: Optional[str] = Field(None, description="MAC address")
    signal_strength: Optional[int] = Field(None, description="Signal strength in dBm")
    latency_ms: Optional[float] = Field(None, description="Latency in milliseconds")
    bandwidth_mbps: Optional[float] = Field(None, description="Bandwidth in Mbps")
    packet_loss_percent: Optional[float] = Field(None, description="Packet loss percentage")
    connection_errors: int = Field(..., description="Connection error count")
    uptime_seconds: int = Field(..., description="Uptime in seconds")
    last_heartbeat: Optional[datetime] = Field(None, description="Last heartbeat timestamp")


# WiFi AP Schemas

class APStartRequest(BaseModel):
    """WiFi AP start request schema"""
    ssid: str = Field(..., min_length=1, max_length=32, description="WiFi AP SSID")
    password: str = Field(..., min_length=8, max_length=63, description="WiFi AP password")
    channel: int = Field(..., ge=1, le=14, description="WiFi channel")
    security: SecurityType = Field(default=SecurityType.WPA2, description="Security type")
    hidden: bool = Field(default=False, description="Hidden network")
    max_clients: int = Field(default=10, ge=1, le=50, description="Maximum clients")
    
    @field_validator('ssid')
    @classmethod
    def validate_ssid(cls, v: str) -> str:
        if not v.strip():
            raise ValueError('SSID cannot be empty')
        return v.strip()
    
    @field_validator('password')
    @classmethod
    def validate_password(cls, v: str) -> str:
        if len(v) < 8:
            raise ValueError('Password must be at least 8 characters long')
        return v


class APStartResponse(BaseModel):
    """WiFi AP start response schema"""
    success: bool = Field(..., description="Operation success status")
    message: str = Field(..., description="Response message")
    data: Optional[Dict[str, Any]] = Field(None, description="AP start data")
    timestamp: datetime = Field(..., description="Response timestamp")


class APStopResponse(BaseModel):
    """WiFi AP stop response schema"""
    success: bool = Field(..., description="Operation success status")
    message: str = Field(..., description="Response message")
    timestamp: datetime = Field(..., description="Response timestamp")


class APConfigRequest(BaseModel):
    """WiFi AP configuration request schema"""
    ssid: Optional[str] = Field(None, min_length=1, max_length=32, description="WiFi AP SSID")
    password: Optional[str] = Field(None, min_length=8, max_length=63, description="WiFi AP password")
    channel: Optional[int] = Field(None, ge=1, le=14, description="WiFi channel")
    security: Optional[SecurityType] = Field(None, description="Security type")
    hidden: Optional[bool] = Field(None, description="Hidden network")
    max_clients: Optional[int] = Field(None, ge=1, le=50, description="Maximum clients")
    bandwidth_limit: Optional[int] = Field(None, ge=1, le=1000, description="Bandwidth limit in Mbps")


class APConfigResponse(BaseModel):
    """WiFi AP configuration response schema"""
    success: bool = Field(..., description="Operation success status")
    message: str = Field(..., description="Response message")
    data: Optional[Dict[str, Any]] = Field(None, description="Configuration data")
    timestamp: datetime = Field(..., description="Response timestamp")


# AP Client Schemas

class APClient(BaseModel):
    """WiFi AP client schema"""
    mac_address: str = Field(..., description="Client MAC address")
    ip_address: Optional[str] = Field(None, description="Client IP address")
    hostname: Optional[str] = Field(None, description="Client hostname")
    device_type: Optional[str] = Field(None, description="Device type")
    signal_strength: Optional[int] = Field(None, description="Signal strength in dBm")
    connected_at: datetime = Field(..., description="Connection timestamp")
    last_seen: datetime = Field(..., description="Last seen timestamp")
    bytes_sent: int = Field(default=0, description="Bytes sent")
    bytes_received: int = Field(default=0, description="Bytes received")
    session_duration: int = Field(default=0, description="Session duration in seconds")
    is_connected: bool = Field(..., description="Connection status")
    is_blocked: bool = Field(default=False, description="Blocked status")


class APClientsResponse(BaseModel):
    """WiFi AP clients response schema"""
    success: bool = Field(..., description="Operation success status")
    data: Dict[str, Any] = Field(..., description="Clients data")
    timestamp: datetime = Field(..., description="Response timestamp")


class APClientsData(BaseModel):
    """WiFi AP clients data schema"""
    clients: List[APClient] = Field(..., description="List of connected clients")
    client_count: int = Field(..., description="Total client count")


# Fallback Connectivity Schemas

class FallbackEnableResponse(BaseModel):
    """Fallback enable response schema"""
    success: bool = Field(..., description="Operation success status")
    message: str = Field(..., description="Response message")
    data: Optional[Dict[str, Any]] = Field(None, description="Fallback data")
    timestamp: datetime = Field(..., description="Response timestamp")


class FallbackDisableResponse(BaseModel):
    """Fallback disable response schema"""
    success: bool = Field(..., description="Operation success status")
    message: str = Field(..., description="Response message")
    timestamp: datetime = Field(..., description="Response timestamp")


# Network Event Schemas

class NetworkEvent(BaseModel):
    """Network event schema"""
    event_type: str = Field(..., description="Event type")
    event_level: EventLevel = Field(..., description="Event level")
    message: str = Field(..., description="Event message")
    event_data: Optional[Dict[str, Any]] = Field(None, description="Event data")
    source: Optional[str] = Field(None, description="Event source")
    client_mac: Optional[str] = Field(None, description="Related client MAC")
    config_id: Optional[int] = Field(None, description="Related configuration ID")
    occurred_at: datetime = Field(..., description="Event timestamp")


class NetworkEventsResponse(BaseModel):
    """Network events response schema"""
    success: bool = Field(..., description="Operation success status")
    events: List[NetworkEvent] = Field(..., description="List of network events")
    total_count: int = Field(..., description="Total event count")
    page: int = Field(..., description="Current page number")
    page_size: int = Field(..., description="Number of items per page")
    has_next: bool = Field(..., description="Whether there are more pages")


# Network Performance Schemas

class NetworkPerformance(BaseModel):
    """Network performance schema"""
    timestamp: datetime = Field(..., description="Performance timestamp")
    interface: NetworkInterface = Field(..., description="Network interface")
    bytes_sent: int = Field(..., description="Bytes sent")
    bytes_received: int = Field(..., description="Bytes received")
    packets_sent: int = Field(..., description="Packets sent")
    packets_received: int = Field(..., description="Packets received")
    latency_ms: Optional[float] = Field(None, description="Latency in milliseconds")
    jitter_ms: Optional[float] = Field(None, description="Jitter in milliseconds")
    packet_loss_percent: Optional[float] = Field(None, description="Packet loss percentage")
    signal_strength: Optional[int] = Field(None, description="Signal strength in dBm")
    noise_level: Optional[int] = Field(None, description="Noise level in dBm")
    snr: Optional[int] = Field(None, description="Signal-to-Noise Ratio")
    connection_count: int = Field(..., description="Total connection count")
    active_connections: int = Field(..., description="Active connection count")


class NetworkPerformanceResponse(BaseModel):
    """Network performance response schema"""
    success: bool = Field(..., description="Operation success status")
    data: List[NetworkPerformance] = Field(..., description="Performance data")
    total_count: int = Field(..., description="Total data count")
    page: int = Field(..., description="Current page number")
    page_size: int = Field(..., description="Number of items per page")
    has_next: bool = Field(..., description="Whether there are more pages")


# Error Response Schema

class NetworkErrorResponse(BaseModel):
    """Network error response schema"""
    success: bool = Field(default=False, description="Operation success status")
    message: str = Field(..., description="Error message")
    error: str = Field(..., description="Error details")
    timestamp: datetime = Field(..., description="Error timestamp")
    error_code: Optional[str] = Field(None, description="Error code")


# WiFi Management Schemas

class WiFiNetwork(BaseModel):
    """WiFi network information schema"""
    ssid: str = Field(..., description="Network SSID")
    bssid: Optional[str] = Field(None, description="Network BSSID")
    signal_strength: Optional[int] = Field(None, description="Signal strength in dBm")
    frequency: Optional[int] = Field(None, description="Frequency in MHz")
    security: Optional[str] = Field(None, description="Security type")
    is_connected: bool = Field(default=False, description="Is currently connected")
    is_saved: bool = Field(default=False, description="Is saved in system")


class WiFiScanResponse(BaseModel):
    """WiFi scan response schema"""
    success: bool = Field(..., description="Operation success status")
    data: Dict[str, Any] = Field(..., description="Scan results data")
    timestamp: datetime = Field(..., description="Response timestamp")
    error: Optional[str] = Field(None, description="Error message if failed")


class WiFiConnectRequest(BaseModel):
    """WiFi connection request schema"""
    ssid: str = Field(..., description="Network SSID")
    password: Optional[str] = Field(None, description="Network password")
    security: Optional[str] = Field(None, description="Security type")


class WiFiConnectResponse(BaseModel):
    """WiFi connection response schema"""
    success: bool = Field(..., description="Operation success status")
    message: str = Field(..., description="Response message")
    data: Optional[Dict[str, Any]] = Field(None, description="Connection data")
    timestamp: datetime = Field(..., description="Response timestamp")


class WiFiDisconnectResponse(BaseModel):
    """WiFi disconnection response schema"""
    success: bool = Field(..., description="Operation success status")
    message: str = Field(..., description="Response message")
    timestamp: datetime = Field(..., description="Response timestamp")


# Network Performance Schemas

class NetworkPerformanceData(BaseModel):
    """Network performance data schema"""
    bandwidth_mbps: Optional[float] = Field(None, description="Bandwidth in Mbps")
    latency_ms: Optional[float] = Field(None, description="Latency in milliseconds")
    packet_loss_percent: Optional[float] = Field(None, description="Packet loss percentage")
    signal_strength_dbm: Optional[int] = Field(None, description="Signal strength in dBm")
    connection_quality: Optional[str] = Field(None, description="Connection quality rating")
    throughput_mbps: Optional[float] = Field(None, description="Throughput in Mbps")
    jitter_ms: Optional[float] = Field(None, description="Jitter in milliseconds")
    uptime_seconds: Optional[int] = Field(None, description="Uptime in seconds")


class NetworkPerformanceResponse(BaseModel):
    """Network performance response schema"""
    success: bool = Field(..., description="Operation success status")
    data: NetworkPerformanceData = Field(..., description="Performance data")
    timestamp: datetime = Field(..., description="Response timestamp")


# Network Health Schemas

class NetworkHealthData(BaseModel):
    """Network health data schema"""
    status: str = Field(..., description="Health status")
    service: str = Field(..., description="Service name")
    firmware_connected: bool = Field(..., description="Firmware connection status")
    connection_errors: int = Field(..., description="Connection error count")
    last_heartbeat: Optional[str] = Field(None, description="Last heartbeat timestamp")
    uptime_seconds: Optional[int] = Field(None, description="Uptime in seconds")
    memory_usage_percent: Optional[float] = Field(None, description="Memory usage percentage")
    cpu_usage_percent: Optional[float] = Field(None, description="CPU usage percentage")


class NetworkHealthResponse(BaseModel):
    """Network health response schema"""
    success: bool = Field(..., description="Operation success status")
    data: Optional[NetworkHealthData] = Field(None, description="Health data")
    error: Optional[str] = Field(None, description="Error message if failed")
    timestamp: datetime = Field(..., description="Response timestamp")


# WiFi IP Configuration Schemas - Issue #216

class WiFiIPMode(str, Enum):
    """WiFi IP mode enumeration"""
    STATIC = "Static"
    DHCP = "DHCP"


class WiFiIPConfigRequest(BaseModel):
    """
    WiFi IP Configuration Request - Issue #206/#216
    
    Request schema for updating WiFi IP configuration (Static or DHCP)
    """
    ip_mode: WiFiIPMode = Field(..., description="IP mode: Static or DHCP")
    ip_address: Optional[str] = Field(
        None, 
        pattern=r"^\d{1,3}\.\d{1,3}\.\d{1,3}\.\d{1,3}$",
        description="IP address (required for Static mode)"
    )
    netmask: Optional[str] = Field(
        None,
        pattern=r"^\d{1,3}\.\d{1,3}\.\d{1,3}\.\d{1,3}$",
        description="Subnet mask (required for Static mode)"
    )
    gateway: Optional[str] = Field(
        None,
        pattern=r"^\d{1,3}\.\d{1,3}\.\d{1,3}\.\d{1,3}$",
        description="Gateway address (required for Static mode)"
    )
    dns: Optional[str] = Field(
        None,
        pattern=r"^\d{1,3}\.\d{1,3}\.\d{1,3}\.\d{1,3}$",
        description="DNS server (optional, default: 8.8.8.8)"
    )
    
    @field_validator('ip_address', 'netmask', 'gateway')
    @classmethod
    def validate_static_required_fields(cls, v: Optional[str], info: ValidationInfo) -> Optional[str]:
        """Validate that IP fields are required for Static mode"""
        if info.data.get('ip_mode') == WiFiIPMode.STATIC:
            if not v:
                raise ValueError(f"{info.field_name} is required for Static mode")
        return v


class WiFiIPConfigData(BaseModel):
    """WiFi IP configuration data"""
    ssid: str = Field(..., description="Connected WiFi SSID")
    ip_mode: str = Field(..., description="IP mode: Static or DHCP")
    signal_strength: int = Field(..., description="Signal strength in dBm")
    ip_address: str = Field(..., description="Current IP address")
    gateway: str = Field(..., description="Gateway address")
    dns: str = Field(..., description="DNS server")
    netmask: str = Field(..., description="Subnet mask")
    mac_address: str = Field(..., description="MAC address")
    security_type: str = Field(..., description="Security type")
    link_speed: int = Field(..., description="Link speed in Mbps")
    uptime_seconds: int = Field(..., description="Connection uptime in seconds")


class WiFiIPConfigResponse(BaseModel):
    """WiFi IP Configuration Response - Issue #206/#216"""
    success: bool = Field(..., description="Operation success status")
    data: Optional[WiFiIPConfigData] = Field(None, description="Updated network info")
    message: str = Field(..., description="Response message")
    timestamp: datetime = Field(..., description="Response timestamp")