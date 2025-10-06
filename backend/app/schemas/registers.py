"""
Pydantic schemas for Registers CRUD with validation
"""

from pydantic import BaseModel, Field, field_validator
from typing import Optional
from enum import Enum


class RegisterMode(str, Enum):
    READ = "READ"
    WRITE = "WRITE"
    READ_WRITE = "READ_WRITE"


class RegisterAccessLevel(str, Enum):
    USER = "USER"
    ADMIN = "ADMIN"
    SYSTEM = "SYSTEM"


def _validate_address(addr: str) -> str:
    if not isinstance(addr, str):
        raise ValueError("address must be string like '0x0001'")
    if not addr.startswith("0x") or len(addr) not in (4, 6):
        # Allow 0x00 or 0x0000 forms
        raise ValueError("address must be hex like '0x00' or '0x0001'")
    try:
        int(addr, 16)
    except Exception:
        raise ValueError("address must be valid hex")
    return addr.lower()


class RegisterBase(BaseModel):
    name: str = Field(..., min_length=1, max_length=100)
    address: str = Field(..., description="Hex address e.g., 0x0001")
    mode: RegisterMode = Field(default=RegisterMode.READ)
    access_level: RegisterAccessLevel = Field(default=RegisterAccessLevel.USER)
    is_safe_register: bool = Field(default=False)
    unit: Optional[str] = Field(default=None, max_length=20)
    description: Optional[str] = Field(default=None, max_length=255)

    @field_validator("address")
    @classmethod
    def validate_address(cls, v: str) -> str:
        return _validate_address(v)


class RegisterCreate(RegisterBase):
    pass


class RegisterUpdate(BaseModel):
    name: Optional[str] = Field(default=None, min_length=1, max_length=100)
    address: Optional[str] = Field(default=None)
    mode: Optional[RegisterMode] = Field(default=None)
    access_level: Optional[RegisterAccessLevel] = Field(default=None)
    is_safe_register: Optional[bool] = Field(default=None)
    unit: Optional[str] = Field(default=None, max_length=20)
    description: Optional[str] = Field(default=None, max_length=255)

    @field_validator("address")
    @classmethod
    def validate_address(cls, v: Optional[str]) -> Optional[str]:
        if v is None:
            return v
        return _validate_address(v)


class RegisterOut(RegisterBase):
    id: int

    class Config:
        from_attributes = True


