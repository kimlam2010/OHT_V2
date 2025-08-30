"""
Pydantic models cho xác thực người dùng
"""

from pydantic import BaseModel, Field, ConfigDict


class LoginRequest(BaseModel):
    model_config = ConfigDict()

    username: str = Field(description="Tên đăng nhập")
    password: str = Field(description="Mật khẩu")


class TokenResponse(BaseModel):
    model_config = ConfigDict()

    token: str = Field(description="JWT token")

"""
Pydantic models cho xác thực người dùng
"""

from typing import Optional

from pydantic import BaseModel, Field, ConfigDict


class LoginRequest(BaseModel):
    model_config = ConfigDict()

    username: str = Field(description="Tên đăng nhập")
    password: str = Field(description="Mật khẩu")


class TokenResponse(BaseModel):
    model_config = ConfigDict()

    token: str = Field(description="JWT token")


class UserRecord(BaseModel):
    model_config = ConfigDict()

    username: str = Field(description="Tên đăng nhập")
    password_hash: str = Field(description="Mật khẩu hash (bcrypt)")
    role: str = Field(default="admin", description="Vai trò")



