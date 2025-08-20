from typing import Literal, Optional
from pydantic import BaseModel, EmailStr, Field


Theme = Literal["light", "dark"]


class UserSettings(BaseModel):
	username: str = Field(min_length=1)
	display_name: Optional[str] = None
	email: EmailStr
	theme: Theme = "light"


class User(BaseModel):
	username: str
	display_name: Optional[str] = None
	email: EmailStr
	role: Literal["admin", "operator"] = "operator"


