"""
Network models for OHT-50 Backend

Database models for network configuration, status, and management.
"""

from sqlalchemy import Column, Integer, String, Boolean, DateTime, Text, JSON, Float
from sqlalchemy.sql import func
from sqlalchemy.ext.declarative import declarative_base
from datetime import datetime
from typing import Dict, Any, Optional, List

Base = declarative_base()


class NetworkConfiguration(Base):
    """Network configuration model"""
    __tablename__ = "network_configurations"
    
    id = Column(Integer, primary_key=True, index=True)
    config_name = Column(String(100), nullable=False, index=True)
    config_type = Column(String(50), nullable=False)  # 'wifi_ap', 'ethernet', 'fallback'
    
    # WiFi AP Configuration
    ssid = Column(String(100), nullable=True)
    password = Column(String(100), nullable=True)
    channel = Column(Integer, nullable=True)
    security = Column(String(20), nullable=True)  # 'WPA2', 'WPA3', 'Open'
    hidden = Column(Boolean, default=False)
    
    # Network Settings
    ip_address = Column(String(15), nullable=True)
    subnet_mask = Column(String(15), nullable=True)
    gateway = Column(String(15), nullable=True)
    dns_servers = Column(JSON, nullable=True)  # List of DNS servers
    
    # Advanced Settings
    max_clients = Column(Integer, default=10)
    bandwidth_limit = Column(Integer, nullable=True)  # Mbps
    auto_start = Column(Boolean, default=False)
    
    # Metadata
    is_active = Column(Boolean, default=False)
    created_at = Column(DateTime(timezone=True), server_default=func.now())
    updated_at = Column(DateTime(timezone=True), onupdate=func.now())
    created_by = Column(String(100), nullable=True)
    
    def to_dict(self) -> Dict[str, Any]:
        """Convert model to dictionary"""
        return {
            "id": self.id,
            "config_name": self.config_name,
            "config_type": self.config_type,
            "ssid": self.ssid,
            "password": "***" if self.password else None,  # Hide password
            "channel": self.channel,
            "security": self.security,
            "hidden": self.hidden,
            "ip_address": self.ip_address,
            "subnet_mask": self.subnet_mask,
            "gateway": self.gateway,
            "dns_servers": self.dns_servers,
            "max_clients": self.max_clients,
            "bandwidth_limit": self.bandwidth_limit,
            "auto_start": self.auto_start,
            "is_active": self.is_active,
            "created_at": self.created_at.isoformat() if self.created_at else None,
            "updated_at": self.updated_at.isoformat() if self.updated_at else None,
            "created_by": self.created_by
        }


class NetworkStatus(Base):
    """Network status model"""
    __tablename__ = "network_status"
    
    id = Column(Integer, primary_key=True, index=True)
    
    # Connection Status
    status = Column(String(20), nullable=False)  # 'connected', 'disconnected', 'ap_mode', 'fallback'
    ap_status = Column(String(20), nullable=True)  # 'stopped', 'running', 'starting', 'stopping'
    fallback_enabled = Column(Boolean, default=False)
    
    # Network Information
    primary_interface = Column(String(20), nullable=True)  # 'ethernet', 'wifi', 'ap'
    ip_address = Column(String(15), nullable=True)
    mac_address = Column(String(17), nullable=True)
    signal_strength = Column(Integer, nullable=True)  # dBm for WiFi
    
    # Performance Metrics
    latency_ms = Column(Float, nullable=True)
    bandwidth_mbps = Column(Float, nullable=True)
    packet_loss_percent = Column(Float, nullable=True)
    
    # Connection Health
    connection_errors = Column(Integer, default=0)
    last_error = Column(Text, nullable=True)
    uptime_seconds = Column(Integer, default=0)
    
    # Timestamps
    last_heartbeat = Column(DateTime(timezone=True), nullable=True)
    status_changed_at = Column(DateTime(timezone=True), server_default=func.now())
    created_at = Column(DateTime(timezone=True), server_default=func.now())
    updated_at = Column(DateTime(timezone=True), onupdate=func.now())
    
    def to_dict(self) -> Dict[str, Any]:
        """Convert model to dictionary"""
        return {
            "id": self.id,
            "status": self.status,
            "ap_status": self.ap_status,
            "fallback_enabled": self.fallback_enabled,
            "primary_interface": self.primary_interface,
            "ip_address": self.ip_address,
            "mac_address": self.mac_address,
            "signal_strength": self.signal_strength,
            "latency_ms": self.latency_ms,
            "bandwidth_mbps": self.bandwidth_mbps,
            "packet_loss_percent": self.packet_loss_percent,
            "connection_errors": self.connection_errors,
            "last_error": self.last_error,
            "uptime_seconds": self.uptime_seconds,
            "last_heartbeat": self.last_heartbeat.isoformat() if self.last_heartbeat else None,
            "status_changed_at": self.status_changed_at.isoformat() if self.status_changed_at else None,
            "created_at": self.created_at.isoformat() if self.created_at else None,
            "updated_at": self.updated_at.isoformat() if self.updated_at else None
        }


class APClient(Base):
    """WiFi AP client model"""
    __tablename__ = "ap_clients"
    
    id = Column(Integer, primary_key=True, index=True)
    
    # Client Information
    mac_address = Column(String(17), nullable=False, index=True)
    ip_address = Column(String(15), nullable=True)
    hostname = Column(String(100), nullable=True)
    device_type = Column(String(50), nullable=True)  # 'mobile', 'laptop', 'tablet', 'unknown'
    
    # Connection Details
    signal_strength = Column(Integer, nullable=True)  # dBm
    connected_at = Column(DateTime(timezone=True), server_default=func.now())
    last_seen = Column(DateTime(timezone=True), server_default=func.now())
    disconnected_at = Column(DateTime(timezone=True), nullable=True)
    
    # Usage Statistics
    bytes_sent = Column(Integer, default=0)
    bytes_received = Column(Integer, default=0)
    session_duration = Column(Integer, default=0)  # seconds
    
    # Status
    is_connected = Column(Boolean, default=True)
    is_blocked = Column(Boolean, default=False)
    
    # Timestamps
    created_at = Column(DateTime(timezone=True), server_default=func.now())
    updated_at = Column(DateTime(timezone=True), onupdate=func.now())
    
    def to_dict(self) -> Dict[str, Any]:
        """Convert model to dictionary"""
        return {
            "id": self.id,
            "mac_address": self.mac_address,
            "ip_address": self.ip_address,
            "hostname": self.hostname,
            "device_type": self.device_type,
            "signal_strength": self.signal_strength,
            "connected_at": self.connected_at.isoformat() if self.connected_at else None,
            "last_seen": self.last_seen.isoformat() if self.last_seen else None,
            "disconnected_at": self.disconnected_at.isoformat() if self.disconnected_at else None,
            "bytes_sent": self.bytes_sent,
            "bytes_received": self.bytes_received,
            "session_duration": self.session_duration,
            "is_connected": self.is_connected,
            "is_blocked": self.is_blocked,
            "created_at": self.created_at.isoformat() if self.created_at else None,
            "updated_at": self.updated_at.isoformat() if self.updated_at else None
        }


class NetworkEvent(Base):
    """Network event log model"""
    __tablename__ = "network_events"
    
    id = Column(Integer, primary_key=True, index=True)
    
    # Event Information
    event_type = Column(String(50), nullable=False)  # 'ap_started', 'ap_stopped', 'client_connected', etc.
    event_level = Column(String(20), nullable=False)  # 'info', 'warning', 'error', 'critical'
    message = Column(Text, nullable=False)
    
    # Event Data
    event_data = Column(JSON, nullable=True)  # Additional event data
    source = Column(String(50), nullable=True)  # 'firmware', 'backend', 'user'
    
    # Related Entities
    client_mac = Column(String(17), nullable=True)
    config_id = Column(Integer, nullable=True)
    
    # Timestamps
    occurred_at = Column(DateTime(timezone=True), server_default=func.now())
    created_at = Column(DateTime(timezone=True), server_default=func.now())
    
    def to_dict(self) -> Dict[str, Any]:
        """Convert model to dictionary"""
        return {
            "id": self.id,
            "event_type": self.event_type,
            "event_level": self.event_level,
            "message": self.message,
            "event_data": self.event_data,
            "source": self.source,
            "client_mac": self.client_mac,
            "config_id": self.config_id,
            "occurred_at": self.occurred_at.isoformat() if self.occurred_at else None,
            "created_at": self.created_at.isoformat() if self.created_at else None
        }


class NetworkPerformance(Base):
    """Network performance metrics model"""
    __tablename__ = "network_performance"
    
    id = Column(Integer, primary_key=True, index=True)
    
    # Performance Metrics
    timestamp = Column(DateTime(timezone=True), server_default=func.now(), index=True)
    interface = Column(String(20), nullable=False)  # 'ethernet', 'wifi', 'ap'
    
    # Bandwidth Metrics
    bytes_sent = Column(Integer, default=0)
    bytes_received = Column(Integer, default=0)
    packets_sent = Column(Integer, default=0)
    packets_received = Column(Integer, default=0)
    
    # Quality Metrics
    latency_ms = Column(Float, nullable=True)
    jitter_ms = Column(Float, nullable=True)
    packet_loss_percent = Column(Float, nullable=True)
    
    # Signal Metrics (for WiFi)
    signal_strength = Column(Integer, nullable=True)  # dBm
    noise_level = Column(Integer, nullable=True)  # dBm
    snr = Column(Integer, nullable=True)  # Signal-to-Noise Ratio
    
    # Connection Metrics
    connection_count = Column(Integer, default=0)
    active_connections = Column(Integer, default=0)
    
    def to_dict(self) -> Dict[str, Any]:
        """Convert model to dictionary"""
        return {
            "id": self.id,
            "timestamp": self.timestamp.isoformat() if self.timestamp else None,
            "interface": self.interface,
            "bytes_sent": self.bytes_sent,
            "bytes_received": self.bytes_received,
            "packets_sent": self.packets_sent,
            "packets_received": self.packets_received,
            "latency_ms": self.latency_ms,
            "jitter_ms": self.jitter_ms,
            "packet_loss_percent": self.packet_loss_percent,
            "signal_strength": self.signal_strength,
            "noise_level": self.noise_level,
            "snr": self.snr,
            "connection_count": self.connection_count,
            "active_connections": self.active_connections
        }
