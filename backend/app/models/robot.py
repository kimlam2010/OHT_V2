"""
Robot data model for OHT-50 Backend
"""

from sqlalchemy import Column, Integer, String, Float, DateTime, Boolean, Text
from sqlalchemy.sql import func
from app.core.database import Base


class Robot(Base):
    """Robot data model"""
    __tablename__ = "robots"
    
    id = Column(Integer, primary_key=True, index=True)
    robot_id = Column(String(50), unique=True, index=True, nullable=False, default="OHT-50-001")
    status = Column(String(20), nullable=False, default="idle")  # idle, moving, paused, error, estop
    mode = Column(String(20), nullable=False, default="auto")  # auto, manual, semi
    position_x = Column(Float, nullable=True)
    position_y = Column(Float, nullable=True)
    position_z = Column(Float, nullable=True)
    velocity_x = Column(Float, nullable=True)
    velocity_y = Column(Float, nullable=True)
    velocity_z = Column(Float, nullable=True)
    battery_level = Column(Integer, nullable=True)  # percentage
    battery_voltage = Column(Float, nullable=True)
    temperature = Column(Float, nullable=True)
    speed = Column(Float, nullable=True)
    safety_status = Column(String(20), default="safe")
    last_command_id = Column(String(50), nullable=True)
    created_at = Column(DateTime(timezone=True), server_default=func.now())
    updated_at = Column(DateTime(timezone=True), server_default=func.now(), onupdate=func.now())


class RobotCommand(Base):
    """Robot command history model"""
    __tablename__ = "robot_commands"
    
    id = Column(Integer, primary_key=True, index=True)
    command_id = Column(String(50), unique=True, nullable=False, index=True)
    user_id = Column(Integer, nullable=False)
    command_type = Column(String(50), nullable=False)  # move, stop, pause, resume, home, set_mode
    command_data = Column(Text, nullable=True)  # JSON parameters
    priority = Column(String(20), default="normal")  # low, normal, high, emergency
    status = Column(String(20), default="pending")  # pending, executing, completed, failed, cancelled
    result = Column(Text, nullable=True)  # JSON result data
    error_message = Column(Text, nullable=True)
    created_at = Column(DateTime(timezone=True), server_default=func.now())
    started_at = Column(DateTime(timezone=True), nullable=True)
    completed_at = Column(DateTime(timezone=True), nullable=True)


class RobotConfiguration(Base):
    """Robot configuration model"""
    __tablename__ = "robot_configurations"
    
    id = Column(Integer, primary_key=True, index=True)
    config_key = Column(String(100), unique=True, nullable=False, index=True)
    config_value = Column(Text, nullable=False)  # JSON configuration data
    config_type = Column(String(20), nullable=False)  # robot, network, sensor, safety
    description = Column(Text, nullable=True)
    is_active = Column(Boolean, default=True)
    created_at = Column(DateTime(timezone=True), server_default=func.now())
    updated_at = Column(DateTime(timezone=True), server_default=func.now(), onupdate=func.now())
    updated_by = Column(Integer, nullable=True)
