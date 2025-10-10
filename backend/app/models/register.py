"""
Register model for storing RS485 register metadata
"""

from sqlalchemy import Column, Integer, String, Boolean, DateTime, Enum
from sqlalchemy.sql import func
from enum import Enum as PyEnum

from app.core.database import Base


class RegisterMode(str, PyEnum):
    READ = "READ"
    WRITE = "WRITE"
    READ_WRITE = "READ_WRITE"


class RegisterAccessLevel(str, PyEnum):
    USER = "USER"
    ADMIN = "ADMIN"
    SYSTEM = "SYSTEM"


class Register(Base):
    __tablename__ = "registers"

    id = Column(Integer, primary_key=True, index=True)
    name = Column(String(100), nullable=False, index=True)
    address = Column(String(50), nullable=False, index=True)
    mode = Column(Enum(RegisterMode), nullable=False, default=RegisterMode.READ)
    access_level = Column(Enum(RegisterAccessLevel), nullable=False, default=RegisterAccessLevel.USER)
    is_safe_register = Column(Boolean, nullable=False, default=False)
    unit = Column(String(20), nullable=True)
    description = Column(String(255), nullable=True)
    
    # Module information (added for Issue #204 enhancement)
    module_addr = Column(Integer, nullable=True, index=True)  # Module address (2=Power, 3=Safety, 4=Motor, 5=Dock)
    module_name = Column(String(50), nullable=True)  # Module name from firmware
    
    created_at = Column(DateTime(timezone=True), server_default=func.now())
    updated_at = Column(DateTime(timezone=True), server_default=func.now(), onupdate=func.now())


