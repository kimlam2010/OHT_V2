"""
Security module for OHT-50 Backend
"""

import json
from datetime import datetime, timedelta
from typing import Optional, Dict, List
from fastapi import HTTPException, Depends, status
from fastapi.security import HTTPBearer, HTTPAuthorizationCredentials
from jose import JWTError, jwt
from passlib.context import CryptContext
from sqlalchemy.ext.asyncio import AsyncSession
from sqlalchemy import select

from app.config import settings
from app.core.database import get_db
from app.models.user import User, UserPermission, Role

# Password hashing
pwd_context = CryptContext(schemes=["bcrypt"], deprecated="auto")

# JWT token scheme
security = HTTPBearer()


def verify_password(plain_password: str, hashed_password: str) -> bool:
    """Verify a password against its hash"""
    return pwd_context.verify(plain_password, hashed_password)


def get_password_hash(password: str) -> str:
    """Hash a password"""
    return pwd_context.hash(password)


def create_access_token(data: dict, expires_delta: Optional[timedelta] = None) -> str:
    """Create JWT access token"""
    to_encode = data.copy()
    if expires_delta:
        expire = datetime.utcnow() + expires_delta
    else:
        expire = datetime.utcnow() + timedelta(minutes=settings.jwt_expiry)
    
    to_encode.update({"exp": expire})
    encoded_jwt = jwt.encode(
        to_encode, 
        settings.jwt_secret, 
        algorithm=settings.jwt_algorithm
    )
    return encoded_jwt


def verify_token(token: str) -> Optional[dict]:
    """Verify JWT token"""
    try:
        payload = jwt.decode(
            token, 
            settings.jwt_secret, 
            algorithms=[settings.jwt_algorithm]
        )
        return payload
    except JWTError:
        return None


async def get_current_user(
    credentials: HTTPAuthorizationCredentials = Depends(security),
    db: AsyncSession = Depends(get_db)
) -> User:
    """Get current authenticated user"""
    token = credentials.credentials
    payload = verify_token(token)
    
    if payload is None:
        raise HTTPException(
            status_code=status.HTTP_401_UNAUTHORIZED,
            detail="Could not validate credentials",
            headers={"WWW-Authenticate": "Bearer"},
        )
    
    user_id: int = payload.get("sub")
    if user_id is None:
        raise HTTPException(
            status_code=status.HTTP_401_UNAUTHORIZED,
            detail="Could not validate credentials",
            headers={"WWW-Authenticate": "Bearer"},
        )
    
    # Get user from database
    result = await db.execute(select(User).where(User.id == user_id))
    user = result.scalar_one_or_none()
    
    if user is None:
        raise HTTPException(
            status_code=status.HTTP_401_UNAUTHORIZED,
            detail="User not found",
            headers={"WWW-Authenticate": "Bearer"},
        )
    
    if not user.is_active:
        raise HTTPException(
            status_code=status.HTTP_401_UNAUTHORIZED,
            detail="User account is disabled",
            headers={"WWW-Authenticate": "Bearer"},
        )
    
    return user


async def get_user_permissions(user_id: int, db: AsyncSession) -> List[str]:
    """Get user permissions"""
    result = await db.execute(
        select(UserPermission).where(
            UserPermission.user_id == user_id,
            UserPermission.is_granted == True
        )
    )
    permissions = result.scalars().all()
    return [f"{p.resource}:{p.action}" for p in permissions]


async def check_permission(
    user: User,
    resource: str,
    action: str,
    db: AsyncSession
) -> bool:
    """Check if user has permission for resource and action"""
    # Get user permissions
    permissions = await get_user_permissions(user.id, db)
    
    # Check for specific permission
    specific_permission = f"{resource}:{action}"
    if specific_permission in permissions:
        return True
    
    # Check for wildcard permission
    wildcard_permission = f"{resource}:*"
    if wildcard_permission in permissions:
        return True
    
    # Check for admin role
    if user.role == "administrator":
        return True
    
    return False


def require_permission(resource: str, action: str):
    """Decorator to require specific permission"""
    async def permission_checker(
        current_user: User = Depends(get_current_user),
        db: AsyncSession = Depends(get_db)
    ) -> User:
        has_permission = await check_permission(current_user, resource, action, db)
        if not has_permission:
            raise HTTPException(
                status_code=status.HTTP_403_FORBIDDEN,
                detail=f"Insufficient permissions for {resource}:{action}"
            )
        return current_user
    return permission_checker


def require_role(required_role: str):
    """Decorator to require specific role"""
    async def role_checker(
        current_user: User = Depends(get_current_user)
    ) -> User:
        if current_user.role != required_role and current_user.role != "administrator":
            raise HTTPException(
                status_code=status.HTTP_403_FORBIDDEN,
                detail=f"Role {required_role} required"
            )
        return current_user
    return role_checker


# Permission constants
PERMISSIONS = {
    # Robot control permissions
    "robot:read": "Read robot status and telemetry",
    "robot:control": "Control robot movement and operations",
    "robot:emergency": "Execute emergency stop",
    "robot:configure": "Configure robot settings",
    
    # Safety permissions
    "safety:read": "Read safety status and logs",
    "safety:acknowledge": "Acknowledge safety alerts",
    "safety:configure": "Configure safety settings",
    
    # System permissions
    "system:read": "Read system status and logs",
    "system:configure": "Configure system settings",
    "system:admin": "Administrative system access",
    
    # User management permissions
    "users:read": "Read user information",
    "users:create": "Create new users",
    "users:update": "Update user information",
    "users:delete": "Delete users",
    
    # Telemetry permissions
    "telemetry:read": "Read telemetry data",
    "telemetry:export": "Export telemetry data",
    "telemetry:configure": "Configure telemetry settings",
}


# Role definitions with permissions
ROLE_PERMISSIONS = {
    "administrator": [
        "robot:read", "robot:control", "robot:emergency", "robot:configure",
        "safety:read", "safety:acknowledge", "safety:configure",
        "system:read", "system:configure", "system:admin",
        "users:read", "users:create", "users:update", "users:delete",
        "telemetry:read", "telemetry:export", "telemetry:configure"
    ],
    "supervisor": [
        "robot:read", "robot:control", "robot:emergency",
        "safety:read", "safety:acknowledge", "safety:configure",
        "system:read", "system:configure",
        "users:read", "users:create", "users:update",
        "telemetry:read", "telemetry:export", "telemetry:configure"
    ],
    "operator": [
        "robot:read", "robot:control", "robot:emergency",
        "safety:read", "safety:acknowledge",
        "system:read",
        "telemetry:read"
    ],
    "viewer": [
        "robot:read",
        "safety:read",
        "system:read",
        "telemetry:read"
    ]
}


async def create_default_roles(db: AsyncSession) -> None:
    """Create default roles and permissions"""
    for role_name, permissions in ROLE_PERMISSIONS.items():
        # Check if role exists
        result = await db.execute(select(Role).where(Role.role_name == role_name))
        role = result.scalar_one_or_none()
        
        if not role:
            # Create role
            role = Role(
                role_name=role_name,
                description=f"Default {role_name} role",
                permissions=json.dumps(permissions),
                is_active=True
            )
            db.add(role)
            await db.commit()
            await db.refresh(role)
            
            print(f"Created role: {role_name} with {len(permissions)} permissions")


async def create_default_admin_user(db: AsyncSession) -> None:
    """Create default administrator user"""
    result = await db.execute(select(User).where(User.username == "admin"))
    admin_user = result.scalar_one_or_none()
    
    if not admin_user:
        admin_user = User(
            username="admin",
            password_hash=get_password_hash("admin123"),  # Change in production
            email="admin@oht50.com",
            role="administrator",
            permissions=json.dumps(ROLE_PERMISSIONS["administrator"]),
            is_active=True
        )
        db.add(admin_user)
        await db.commit()
        await db.refresh(admin_user)
        
        print("Created default admin user: admin/admin123")
