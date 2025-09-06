"""
Safety events model for OHT-50 Backend
"""

from sqlalchemy import Column, Integer, String, Float, DateTime, Boolean, Text
from sqlalchemy.sql import func
from app.core.database import Base


class SafetyStatus(Base):
    """Safety status model"""
    __tablename__ = "safety_status"
    
    id = Column(Integer, primary_key=True, index=True)
    robot_id = Column(String(50), nullable=False, default="OHT-50-001", index=True)
    emergency_stop = Column(Boolean, default=False, index=True)
    safety_zones = Column(Text, nullable=True)  # JSON safety zone status
    obstacles = Column(Text, nullable=True)  # JSON obstacle data
    safety_switches = Column(Text, nullable=True)  # JSON safety switch status
    safety_score = Column(Integer, nullable=True)  # 0-100 safety score
    last_check = Column(DateTime(timezone=True), server_default=func.now(), index=True)
    created_at = Column(DateTime(timezone=True), server_default=func.now())


class SafetyLog(Base):
    """Safety log model"""
    __tablename__ = "safety_logs"
    
    id = Column(Integer, primary_key=True, index=True)
    robot_id = Column(String(50), nullable=False, default="OHT-50-001", index=True)
    event_type = Column(String(50), nullable=False, index=True)  # estop_activated, zone_violation, switch_triggered
    event_data = Column(Text, nullable=True)  # JSON event data
    severity = Column(String(20), nullable=False, index=True)  # info, warning, error, critical
    user_id = Column(Integer, nullable=True)  # user who triggered or acknowledged
    timestamp = Column(DateTime(timezone=True), server_default=func.now(), index=True)


class SafetyAlert(Base):
    """Safety alert model"""
    __tablename__ = "safety_alerts"
    
    id = Column(Integer, primary_key=True, index=True)
    alert_id = Column(String(50), unique=True, nullable=False, index=True)
    robot_id = Column(String(50), nullable=False, default="OHT-50-001", index=True)
    alert_type = Column(String(50), nullable=False, index=True)  # emergency_stop, zone_violation, obstacle_detected
    severity = Column(String(20), nullable=False, index=True)  # info, warning, error, critical
    message = Column(Text, nullable=False)
    alert_data = Column(Text, nullable=True)  # JSON additional data
    status = Column(String(20), default="active", index=True)  # active, acknowledged, resolved
    acknowledged_by = Column(Integer, nullable=True)
    acknowledged_at = Column(DateTime(timezone=True), nullable=True)
    resolved_at = Column(DateTime(timezone=True), nullable=True)
    created_at = Column(DateTime(timezone=True), server_default=func.now(), index=True)


class SafetyZone(Base):
    """Safety zone model"""
    __tablename__ = "safety_zones"
    
    id = Column(Integer, primary_key=True, index=True)
    zone_id = Column(String(50), unique=True, nullable=False, index=True)
    zone_name = Column(String(100), nullable=False)
    zone_type = Column(String(20), nullable=False, index=True)  # critical, warning, safe
    min_distance = Column(Float, nullable=False)
    max_distance = Column(Float, nullable=False)
    action = Column(String(50), nullable=False)  # emergency_stop, reduce_speed, monitor
    is_active = Column(Boolean, default=True)
    created_at = Column(DateTime(timezone=True), server_default=func.now())
    updated_at = Column(DateTime(timezone=True), server_default=func.now(), onupdate=func.now())
