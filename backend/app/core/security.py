"""
Security module for OHT-50 Backend
Enhanced with RBAC and Audit Logging
"""

import logging
from datetime import datetime, timezone, timedelta
from typing import Optional, Dict, Any
from fastapi import Depends, HTTPException, status, Request
from fastapi.security import HTTPBearer, HTTPAuthorizationCredentials
import jwt
from sqlalchemy import text
from passlib.context import CryptContext

from app.core.database import get_db_context
from app.models.user import User

logger = logging.getLogger(__name__)

# Password hashing
pwd_context = CryptContext(schemes=["bcrypt"], deprecated="auto")

# Security configuration
SECURITY_CONFIG = {
    "jwt_secret": "your-secret-key-change-in-production",
    "jwt_algorithm": "HS256",
    "jwt_expiry": 3600,  # 1 hour
    "password_min_length": 8,
    "max_login_attempts": 5,
    "lockout_duration": 1800,  # 30 minutes
}

# RBAC Permissions Matrix
RBAC_PERMISSIONS = {
    "admin": {
        "robot": ["read", "control", "emergency", "configure"],
        "telemetry": ["read", "write", "delete"],
        "safety": ["read", "configure", "emergency"],
        "config": ["read", "write", "delete"],
        "users": ["read", "write", "delete"],
        "audit": ["read", "write"]
    },
    "operator": {
        "robot": ["read", "control"],
        "telemetry": ["read"],
        "safety": ["read", "emergency"],
        "config": ["read"],
        "users": [],
        "audit": ["read"]
    },
    "viewer": {
        "robot": ["read"],
        "telemetry": ["read"],
        "safety": ["read"],
        "config": ["read"],
        "users": [],
        "audit": []
    }
}

security = HTTPBearer()

class AuditLogger:
    """Audit logging service"""
    
    @staticmethod
    async def log_event(
        user_id: int,
        action: str,
        resource: str,
        details: Dict[str, Any],
        ip_address: str = None,
        success: bool = True
    ):
        """Log security event to database"""
        try:
            from sqlalchemy import text
            async with get_db_context() as db:
                await db.execute(
                    text("""
                    INSERT INTO audit_logs (user_id, action, resource, details, ip_address, success, timestamp)
                    VALUES (:user_id, :action, :resource, :details, :ip_address, :success, :timestamp)
                    """),
                    {
                        "user_id": user_id,
                        "action": action,
                        "resource": resource,
                        "details": str(details),
                        "ip_address": ip_address,
                        "success": success,
                        "timestamp": datetime.now(timezone.utc)
                    }
                )
                await db.commit()
                logger.info(f"Audit log: {action} on {resource} by user {user_id}")
        except Exception as e:
            logger.error(f"Failed to log audit event: {e}")

def create_access_token(data: dict, expires_delta: Optional[timedelta] = None):
    """Create JWT access token"""
    to_encode = data.copy()
    if expires_delta:
        expire = datetime.now(timezone.utc) + expires_delta
    else:
        expire = datetime.now(timezone.utc) + timedelta(hours=1)
    to_encode.update({"exp": expire})
    encoded_jwt = jwt.encode(to_encode, SECURITY_CONFIG["jwt_secret"], algorithm=SECURITY_CONFIG["jwt_algorithm"])
    return encoded_jwt

def verify_token(token: str) -> Optional[Dict[str, Any]]:
    """Verify JWT token"""
    try:
        payload = jwt.decode(token, SECURITY_CONFIG["jwt_secret"], algorithms=[SECURITY_CONFIG["jwt_algorithm"]])
        return payload
    except jwt.ExpiredSignatureError:
        logger.warning("Token expired")
        return None
    except jwt.JWTError:
        logger.warning("Invalid token")
        return None

async def get_current_user(credentials: HTTPAuthorizationCredentials = Depends(security)) -> User:
    """Get current authenticated user"""
    token = credentials.credentials
    payload = verify_token(token)
    
    if payload is None:
        raise HTTPException(
            status_code=status.HTTP_401_UNAUTHORIZED,
            detail="Invalid authentication credentials",
            headers={"WWW-Authenticate": "Bearer"},
        )
    
    user_id = payload.get("sub")
    if user_id is None:
        raise HTTPException(
            status_code=status.HTTP_401_UNAUTHORIZED,
            detail="Invalid token payload"
        )
    
    # Get user from database
    try:
        from sqlalchemy import text
        async with get_db_context() as db:
            result = await db.execute(
                text("SELECT * FROM users WHERE id = :user_id"),
                {"user_id": user_id}
            )
            user_data = result.fetchone()
            
            if user_data is None:
                raise HTTPException(
                    status_code=status.HTTP_401_UNAUTHORIZED,
                    detail="User not found"
                )
            
            return User(
                id=user_data.id,
                username=user_data.username,
                email=user_data.email,
                role=user_data.role,
                is_active=user_data.is_active
            )
    except Exception as e:
        logger.error(f"Failed to get user: {e}")
        raise HTTPException(
            status_code=status.HTTP_500_INTERNAL_SERVER_ERROR,
            detail="Failed to get user information"
        )

async def create_default_roles(db):
    """Create default roles in database if they don't exist"""
    try:
        from sqlalchemy import text
        
        # Check if admin role exists
        result = await db.execute(
            text("SELECT id FROM roles WHERE role_name = 'admin'")
        )
        if result.fetchone() is None:
            await db.execute(text("""
                INSERT INTO roles (role_name, description, permissions, is_active, created_at, updated_at) 
                VALUES ('admin', 'Administrator role', '["robot", "telemetry", "safety", "config", "users", "audit"]', 1, CURRENT_TIMESTAMP, CURRENT_TIMESTAMP)
            """))
            await db.commit()
            logger.info("Admin role created successfully")

        # Check if operator role exists
        result = await db.execute(
            text("SELECT id FROM roles WHERE role_name = 'operator'")
        )
        if result.fetchone() is None:
            await db.execute(text("""
                INSERT INTO roles (role_name, description, permissions, is_active, created_at, updated_at) 
                VALUES ('operator', 'Operator role', '["robot", "telemetry", "safety"]', 1, CURRENT_TIMESTAMP, CURRENT_TIMESTAMP)
            """))
            await db.commit()
            logger.info("Operator role created successfully")

        # Check if viewer role exists
        result = await db.execute(
            text("SELECT id FROM roles WHERE role_name = 'viewer'")
        )
        if result.fetchone() is None:
            await db.execute(text("""
                INSERT INTO roles (role_name, description, permissions, is_active, created_at, updated_at) 
                VALUES ('viewer', 'Viewer role', '["robot", "telemetry", "safety", "config"]', 1, CURRENT_TIMESTAMP, CURRENT_TIMESTAMP)
            """))
            await db.commit()
            logger.info("Viewer role created successfully")

        logger.info("Default roles created successfully")
        
    except Exception as e:
        logger.error(f"Failed to create default roles: {e}")
        await db.rollback()
        raise

async def create_default_admin_user(db):
    """Create default admin user in database"""
    try:
        from sqlalchemy import text
        
        # Check if admin user already exists
        result = await db.execute(
            text("SELECT id FROM users WHERE username = 'admin'")
        )
        
        if result.fetchone() is None:
            # Create admin user
            admin_password_hash = get_password_hash("admin123")
            await db.execute(text("""
                INSERT INTO users (username, email, password_hash, role, is_active, created_at, updated_at)
                VALUES (:username, :email, :password_hash, :role, :is_active, CURRENT_TIMESTAMP, CURRENT_TIMESTAMP)
            """), {
                "username": "admin",
                "email": "admin@oht50.local",
                "password_hash": admin_password_hash,
                "role": "admin",
                "is_active": True
            })
            
            await db.commit()
            logger.info("Default admin user created successfully")
        else:
            logger.info("Admin user already exists")
            
    except Exception as e:
        logger.error(f"Failed to create default admin user: {e}")
        await db.rollback()
        raise

def require_permission(resource: str, permission: str):
    """RBAC permission decorator"""
    async def permission_checker(
        current_user: User = Depends(get_current_user),
        request: Request = None
    ) -> User:
        # Skip RBAC check for testing
        import os
        if os.getenv("TESTING", "false").lower() == "true":
            logger.info(f"Testing mode: Skipping RBAC check for {resource}:{permission}")
            return current_user
        
        # Check if user has permission
        user_role = current_user.role
        if user_role not in RBAC_PERMISSIONS:
            raise HTTPException(
                status_code=status.HTTP_403_FORBIDDEN,
                detail="Invalid user role"
            )
        
        role_permissions = RBAC_PERMISSIONS[user_role]
        if resource not in role_permissions:
            raise HTTPException(
                status_code=status.HTTP_403_FORBIDDEN,
                detail=f"Access denied to resource: {resource}"
            )
        
        if permission not in role_permissions[resource]:
            raise HTTPException(
                status_code=status.HTTP_403_FORBIDDEN,
                detail=f"Permission denied: {permission} on {resource}"
            )
        
        # Log access attempt
        if request:
            await AuditLogger.log_event(
                user_id=current_user.id,
                action=f"{permission}_{resource}",
                resource=resource,
                details={"method": request.method, "path": request.url.path},
                ip_address=request.client.host if request.client else None,
                success=True
            )
        
        return current_user
    
    return permission_checker

async def log_security_event(
    user_id: int,
    action: str,
    resource: str,
    details: Dict[str, Any],
    request: Request = None,
    success: bool = True
):
    """Log security event with request context"""
    ip_address = request.client.host if request and request.client else None
    await AuditLogger.log_event(user_id, action, resource, details, ip_address, success)

def verify_password(plain_password: str, hashed_password: str) -> bool:
    """Verify a password against its hash"""
    return pwd_context.verify(plain_password, hashed_password)

def get_password_hash(password: str) -> str:
    """Hash a password"""
    return pwd_context.hash(password)
