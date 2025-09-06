"""
Sensor data models for OHT-50 Backend
"""

from sqlalchemy import Column, Integer, String, Float, DateTime, Boolean, Text, JSON
from sqlalchemy.sql import func
from app.core.database import Base


class SensorConfiguration(Base):
    """Sensor configuration model"""
    __tablename__ = "sensor_configurations"
    
    id = Column(Integer, primary_key=True, index=True)
    sensor_id = Column(String(50), unique=True, nullable=False, index=True)
    sensor_type = Column(String(20), nullable=False)  # rfid, accelerometer, proximity, lidar
    name = Column(String(100), nullable=False)
    description = Column(Text, nullable=True)
    configuration = Column(JSON, nullable=False)  # Sensor-specific configuration
    calibration_data = Column(JSON, nullable=True)  # Calibration parameters
    is_active = Column(Boolean, default=True)
    last_calibration = Column(DateTime(timezone=True), nullable=True)
    created_at = Column(DateTime(timezone=True), server_default=func.now())
    updated_at = Column(DateTime(timezone=True), server_default=func.now(), onupdate=func.now())


class SensorReading(Base):
    """Sensor reading model"""
    __tablename__ = "sensor_readings"
    
    id = Column(Integer, primary_key=True, index=True)
    sensor_id = Column(String(50), nullable=False, index=True)
    sensor_type = Column(String(20), nullable=False)
    reading_data = Column(JSON, nullable=False)  # Raw sensor data
    processed_data = Column(JSON, nullable=True)  # Processed/filtered data
    quality_score = Column(Float, nullable=True)  # Data quality 0.0 to 1.0
    is_valid = Column(Boolean, default=True)
    error_code = Column(String(20), nullable=True)
    timestamp = Column(DateTime(timezone=True), server_default=func.now())


class SensorStatus(Base):
    """Sensor status model"""
    __tablename__ = "sensor_status"
    
    id = Column(Integer, primary_key=True, index=True)
    sensor_id = Column(String(50), unique=True, nullable=False, index=True)
    sensor_type = Column(String(20), nullable=False)
    status = Column(String(20), nullable=False, default="unknown")  # online, offline, error, calibrating
    last_reading_time = Column(DateTime(timezone=True), nullable=True)
    error_count = Column(Integer, default=0)
    last_error = Column(Text, nullable=True)
    health_score = Column(Float, default=1.0)  # 0.0 to 1.0
    updated_at = Column(DateTime(timezone=True), server_default=func.now(), onupdate=func.now())


class SensorCalibration(Base):
    """Sensor calibration model"""
    __tablename__ = "sensor_calibrations"
    
    id = Column(Integer, primary_key=True, index=True)
    sensor_id = Column(String(50), nullable=False, index=True)
    sensor_type = Column(String(20), nullable=False)
    calibration_type = Column(String(20), nullable=False)  # offset, scale, rotation, etc.
    calibration_data = Column(JSON, nullable=False)
    reference_data = Column(JSON, nullable=True)
    accuracy = Column(Float, nullable=True)  # Calibration accuracy
    is_valid = Column(Boolean, default=True)
    performed_by = Column(Integer, nullable=True)
    created_at = Column(DateTime(timezone=True), server_default=func.now())
    expires_at = Column(DateTime(timezone=True), nullable=True)
