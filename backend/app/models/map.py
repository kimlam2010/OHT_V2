"""
Map & Localization data models for OHT-50 Backend
"""

from sqlalchemy import Column, Integer, String, Float, DateTime, Boolean, Text, ForeignKey, JSON
from sqlalchemy.sql import func
from sqlalchemy.orm import relationship
from app.core.database import Base


class Map(Base):
    """Map data model"""
    __tablename__ = "maps"
    
    id = Column(Integer, primary_key=True, index=True)
    map_id = Column(String(50), unique=True, nullable=False, index=True)
    name = Column(String(100), nullable=False)
    resolution = Column(Float, nullable=False, default=50.0)  # mm per pixel
    width = Column(Integer, nullable=False, default=1000)  # pixels
    height = Column(Integer, nullable=False, default=1000)  # pixels
    occupancy_grid = Column(JSON, nullable=False)  # 2D array of occupancy values
    robot_trajectory = Column(JSON, default=list)  # List of robot positions during mapping
    rfid_positions = Column(JSON, default=list)  # List of RFID tag positions
    map_metadata = Column(JSON, default=dict)  # Additional map metadata
    is_active = Column(Boolean, default=True)
    created_at = Column(DateTime(timezone=True), server_default=func.now())
    updated_at = Column(DateTime(timezone=True), server_default=func.now(), onupdate=func.now())
    
    # Relationships
    sessions = relationship("MapSession", back_populates="map")
    positions = relationship("RobotPosition", back_populates="map")


class MapSession(Base):
    """Map session tracking model"""
    __tablename__ = "map_sessions"
    
    id = Column(Integer, primary_key=True, index=True)
    session_id = Column(String(50), unique=True, nullable=False, index=True)
    map_id = Column(String(50), ForeignKey("maps.map_id"), nullable=False)
    start_time = Column(DateTime(timezone=True), server_default=func.now())
    end_time = Column(DateTime(timezone=True), nullable=True)
    is_active = Column(Boolean, default=True)
    total_scans = Column(Integer, default=0)
    mapping_quality = Column(Float, default=0.0)  # 0.0 to 1.0
    created_at = Column(DateTime(timezone=True), server_default=func.now())
    
    # Relationships
    map = relationship("Map", back_populates="sessions")
    positions = relationship("RobotPosition", back_populates="session")


class RobotPosition(Base):
    """Robot position tracking model"""
    __tablename__ = "robot_positions"
    
    id = Column(Integer, primary_key=True, index=True)
    map_id = Column(String(50), ForeignKey("maps.map_id"), nullable=False)
    session_id = Column(String(50), ForeignKey("map_sessions.session_id"), nullable=True)
    x = Column(Float, nullable=False)  # X coordinate in mm
    y = Column(Float, nullable=False)  # Y coordinate in mm
    theta = Column(Float, nullable=False)  # Orientation in radians
    confidence = Column(Float, nullable=False)  # Localization confidence 0.0 to 1.0
    source = Column(String(20), nullable=False)  # rfid, accelerometer, proximity, lidar, hybrid
    timestamp = Column(DateTime(timezone=True), server_default=func.now())
    
    # Relationships
    map = relationship("Map", back_populates="positions")
    session = relationship("MapSession", back_populates="positions")


class RFIDTag(Base):
    """RFID tag position model"""
    __tablename__ = "rfid_tags"
    
    id = Column(Integer, primary_key=True, index=True)
    rfid_id = Column(String(50), unique=True, nullable=False, index=True)
    position_x = Column(Float, nullable=False)  # X coordinate in mm
    position_y = Column(Float, nullable=False)  # Y coordinate in mm
    orientation = Column(Float, default=0.0)  # Orientation in radians
    description = Column(Text, nullable=True)
    is_active = Column(Boolean, default=True)
    created_at = Column(DateTime(timezone=True), server_default=func.now())
    updated_at = Column(DateTime(timezone=True), server_default=func.now(), onupdate=func.now())




class LocalizationConfig(Base):
    """Localization configuration model"""
    __tablename__ = "localization_configs"
    
    id = Column(Integer, primary_key=True, index=True)
    config_key = Column(String(100), unique=True, nullable=False, index=True)
    config_value = Column(JSON, nullable=False)
    config_type = Column(String(20), nullable=False)  # rfid, accelerometer, proximity, lidar, fusion
    description = Column(Text, nullable=True)
    is_active = Column(Boolean, default=True)
    created_at = Column(DateTime(timezone=True), server_default=func.now())
    updated_at = Column(DateTime(timezone=True), server_default=func.now(), onupdate=func.now())
    updated_by = Column(Integer, nullable=True)
