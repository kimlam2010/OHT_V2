"""
Telemetry data model for OHT-50 Backend
"""

from sqlalchemy import Column, Integer, String, Float, DateTime, Text, Boolean
from sqlalchemy.sql import func
from app.core.database import Base


class TelemetryCurrent(Base):
    """Current telemetry data model"""
    __tablename__ = "telemetry_current"
    
    id = Column(Integer, primary_key=True, index=True)
    robot_id = Column(String(50), nullable=False, default="OHT-50-001", index=True)
    telemetry_type = Column(String(20), nullable=False, index=True)  # robot, system, sensor
    telemetry_data = Column(Text, nullable=False)  # JSON telemetry data
    timestamp = Column(DateTime(timezone=True), server_default=func.now(), index=True)


class TelemetryHistory(Base):
    """Historical telemetry data model"""
    __tablename__ = "telemetry_history"
    
    id = Column(Integer, primary_key=True, index=True)
    robot_id = Column(String(50), nullable=False, default="OHT-50-001", index=True)
    telemetry_type = Column(String(20), nullable=False, index=True)
    telemetry_data = Column(Text, nullable=False)  # JSON telemetry data
    timestamp = Column(DateTime(timezone=True), server_default=func.now(), index=True)


class TelemetryAlert(Base):
    """Telemetry alert model"""
    __tablename__ = "telemetry_alerts"
    
    id = Column(Integer, primary_key=True, index=True)
    alert_id = Column(String(50), unique=True, nullable=False, index=True)
    robot_id = Column(String(50), nullable=False, default="OHT-50-001", index=True)
    alert_type = Column(String(50), nullable=False, index=True)  # battery, temperature, safety, system
    severity = Column(String(20), nullable=False, index=True)  # info, warning, error, critical
    message = Column(Text, nullable=False)
    alert_data = Column(Text, nullable=True)  # JSON additional data
    status = Column(String(20), default="active", index=True)  # active, acknowledged, resolved
    acknowledged_by = Column(Integer, nullable=True)
    acknowledged_at = Column(DateTime(timezone=True), nullable=True)
    resolved_at = Column(DateTime(timezone=True), nullable=True)
    created_at = Column(DateTime(timezone=True), server_default=func.now(), index=True)


class SensorData(Base):
    """Sensor data model"""
    __tablename__ = "sensor_data"
    
    id = Column(Integer, primary_key=True, index=True)
    robot_id = Column(String(50), nullable=False, default="OHT-50-001", index=True)
    sensor_type = Column(String(50), nullable=False, index=True)  # proximity, nfc, accelerometer, compass
    sensor_id = Column(String(50), nullable=False, index=True)
    sensor_value = Column(Text, nullable=False)  # JSON sensor data
    quality = Column(Float, nullable=True)  # Data quality score 0-1
    is_valid = Column(Boolean, default=True)
    timestamp = Column(DateTime(timezone=True), server_default=func.now(), index=True)
