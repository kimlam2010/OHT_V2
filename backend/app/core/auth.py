"""
Authentication and authorization utilities for OHT-50 Backend

NOTE:
- The canonical `get_current_user` implementation lives in `app.core.security`.
- This module re-exports token helpers from `app.core.security` to avoid duplication.
"""

import logging
from typing import Optional
from fastapi import Depends, HTTPException, status
from fastapi.security import HTTPAuthorizationCredentials
from sqlalchemy.orm import Session

from app.core.database import get_db
from app.core.security import (
    verify_password,
    get_current_user as security_get_current_user,
    create_access_token,
    create_refresh_token,
    verify_token,
)
from app.models.user import User

logger = logging.getLogger(__name__)

# Re-export for backward compatibility (avoid breaking legacy imports)
get_current_user = security_get_current_user


async def get_current_active_user(current_user: User = Depends(get_current_user)) -> User:
    """Get current active user (wraps security.get_current_user)"""
    if not current_user.is_active:
        raise HTTPException(
            status_code=status.HTTP_400_BAD_REQUEST,
            detail="Inactive user"
        )
    return current_user


async def get_current_admin_user(current_user: User = Depends(get_current_active_user)) -> User:
    """Get current admin user"""
    if current_user.role != "admin":
        raise HTTPException(
            status_code=status.HTTP_403_FORBIDDEN,
            detail="Not enough permissions"
        )
    return current_user


def authenticate_user(db: Session, username: str, password: str) -> Optional[User]:
    """Authenticate user with username and password"""
    user = db.query(User).filter(User.username == username).first()
    if not user:
        return None
    if not verify_password(password, user.password_hash):
        return None
    return user
