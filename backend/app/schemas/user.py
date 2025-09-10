from pydantic import BaseModel, EmailStr, Field, field_validator, ConfigDict, ValidationInfo
from typing import Optional
from datetime import datetime
from enum import Enum

class UserRole(str, Enum):
    ADMIN = "admin"
    OPERATOR = "operator"
    VIEWER = "viewer"

class UserStatus(str, Enum):
    ACTIVE = "active"
    INACTIVE = "inactive"
    SUSPENDED = "suspended"

class UserBase(BaseModel):
    username: str = Field(..., min_length=3, max_length=50, description="Username must be 3-50 characters")
    email: EmailStr = Field(..., description="Valid email address")
    full_name: str = Field(..., min_length=2, max_length=100, description="Full name must be 2-100 characters")
    role: UserRole = Field(default=UserRole.VIEWER, description="User role in system")
    status: UserStatus = Field(default=UserStatus.ACTIVE, description="User account status")

class UserCreate(UserBase):
    password: str = Field(..., min_length=12, description="Password must be at least 12 characters")
    confirm_password: str = Field(..., description="Password confirmation")
    
    @field_validator('confirm_password')
    @classmethod
    def passwords_match(cls, v: str, info: ValidationInfo) -> str:
        if 'password' in info.data and v != info.data['password']:
            raise ValueError('Passwords do not match')
        return v
    
    @field_validator('password')
    @classmethod
    def password_strength(cls, v: str) -> str:
        if not any(c.isupper() for c in v):
            raise ValueError('Password must contain at least one uppercase letter')
        if not any(c.islower() for c in v):
            raise ValueError('Password must contain at least one lowercase letter')
        if not any(c.isdigit() for c in v):
            raise ValueError('Password must contain at least one digit')
        return v

class UserUpdate(BaseModel):
    username: Optional[str] = Field(None, min_length=3, max_length=50)
    email: Optional[EmailStr] = None
    full_name: Optional[str] = Field(None, min_length=2, max_length=100)
    role: Optional[UserRole] = None
    status: Optional[UserStatus] = None
    password: Optional[str] = Field(None, min_length=8)

class UserResponse(UserBase):
    id: int
    created_at: datetime
    updated_at: datetime
    last_login: Optional[datetime] = None
    
    model_config = ConfigDict(from_attributes=True)

class UserLogin(BaseModel):
    username: str = Field(..., description="Username or email")
    password: str = Field(..., description="User password")

class UserPasswordChange(BaseModel):
    current_password: str = Field(..., description="Current password")
    new_password: str = Field(..., min_length=12, description="New password (min 12 chars)")
    confirm_new_password: str = Field(..., description="Confirm new password")
    
    @field_validator('confirm_new_password')
    @classmethod
    def passwords_match(cls, v: str, info: ValidationInfo) -> str:
        if 'new_password' in info.data and v != info.data['new_password']:
            raise ValueError('New passwords do not match')
        return v

class UserProfile(BaseModel):
    id: int
    username: str
    email: str
    full_name: str
    role: UserRole
    status: UserStatus
    created_at: datetime
    last_login: Optional[datetime] = None
    
    model_config = ConfigDict(from_attributes=True)
