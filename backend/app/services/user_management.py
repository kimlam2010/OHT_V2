"""
User Management Service for Production Environment

This module provides comprehensive user management functionality.
It includes CRUD operations, role assignment, and user validation.
"""

import logging
from typing import Dict, Any, List, Optional
from datetime import datetime, timedelta, timezone
from sqlalchemy.ext.asyncio import AsyncSession
from sqlalchemy import select, update, delete
from fastapi import HTTPException, status

from app.models.user import User
from app.schemas.user import UserCreate, UserUpdate
from app.core.security import get_password_hash, verify_password
from app.core.production_rbac import rbac

logger = logging.getLogger(__name__)


class UserManagementService:
    """
    Production-grade user management service
    
    Features:
    - User CRUD operations
    - Role assignment management
    - Password validation
    - User session management
    - Security policies
    """
    
    def __init__(self, db: AsyncSession):
        self.db = db
        
    async def create_user(self, user_data: UserCreate) -> User:
        """Create new user with role assignment"""
        try:
            # Check if username already exists
            existing_user = await self.get_user_by_username(user_data.username)
            if existing_user:
                raise HTTPException(
                    status_code=status.HTTP_400_BAD_REQUEST,
                    detail="Username already exists"
                )
                
            # Validate role
            if not self._is_valid_role(user_data.role):
                raise HTTPException(
                    status_code=status.HTTP_400_BAD_REQUEST,
                    detail=f"Invalid role: {user_data.role}"
                )
                
            # Hash password
            hashed_password = get_password_hash(user_data.password)
            
            # Create user
            user = User(
                username=user_data.username,
                email=user_data.email,
                hashed_password=hashed_password,
                role=user_data.role,
                is_active=True,
                created_at=datetime.now(timezone.utc)
            )
            
            self.db.add(user)
            await self.db.commit()
            await self.db.refresh(user)
            
            logger.info(f"✅ User created: {user_data.username} with role {user_data.role}")
            return user
            
        except HTTPException:
            raise
        except Exception as e:
            logger.error(f"❌ User creation failed: {e}")
            await self.db.rollback()
            raise HTTPException(
                status_code=status.HTTP_500_INTERNAL_SERVER_ERROR,
                detail="User creation failed"
            )
            
    async def get_user_by_id(self, user_id: int) -> Optional[User]:
        """Get user by ID"""
        try:
            result = await self.db.execute(select(User).where(User.id == user_id))
            return result.scalar_one_or_none()
        except Exception as e:
            logger.error(f"❌ Get user by ID failed: {e}")
            return None
            
    async def get_user_by_username(self, username: str) -> Optional[User]:
        """Get user by username"""
        try:
            result = await self.db.execute(select(User).where(User.username == username))
            return result.scalar_one_or_none()
        except Exception as e:
            logger.error(f"❌ Get user by username failed: {e}")
            return None
            
    async def get_user_by_email(self, email: str) -> Optional[User]:
        """Get user by email"""
        try:
            result = await self.db.execute(select(User).where(User.email == email))
            return result.scalar_one_or_none()
        except Exception as e:
            logger.error(f"❌ Get user by email failed: {e}")
            return None
            
    async def get_all_users(self, skip: int = 0, limit: int = 100) -> List[User]:
        """Get all users with pagination"""
        try:
            result = await self.db.execute(
                select(User).offset(skip).limit(limit)
            )
            return result.scalars().all()
        except Exception as e:
            logger.error(f"❌ Get all users failed: {e}")
            return []
            
    async def get_users_by_role(self, role: str) -> List[User]:
        """Get users by role"""
        try:
            result = await self.db.execute(
                select(User).where(User.role == role)
            )
            return result.scalars().all()
        except Exception as e:
            logger.error(f"❌ Get users by role failed: {e}")
            return []
            
    async def update_user(self, user_id: int, user_data: UserUpdate) -> Optional[User]:
        """Update user information"""
        try:
            # Get existing user
            user = await self.get_user_by_id(user_id)
            if not user:
                raise HTTPException(
                    status_code=status.HTTP_404_NOT_FOUND,
                    detail="User not found"
                )
                
            # Update fields
            update_data = {}
            
            if user_data.username is not None:
                # Check if new username already exists
                existing_user = await self.get_user_by_username(user_data.username)
                if existing_user and existing_user.id != user_id:
                    raise HTTPException(
                        status_code=status.HTTP_400_BAD_REQUEST,
                        detail="Username already exists"
                    )
                update_data["username"] = user_data.username
                
            if user_data.email is not None:
                # Check if new email already exists
                existing_user = await self.get_user_by_email(user_data.email)
                if existing_user and existing_user.id != user_id:
                    raise HTTPException(
                        status_code=status.HTTP_400_BAD_REQUEST,
                        detail="Email already exists"
                    )
                update_data["email"] = user_data.email
                
            if user_data.role is not None:
                # Validate role
                if not self._is_valid_role(user_data.role):
                    raise HTTPException(
                        status_code=status.HTTP_400_BAD_REQUEST,
                        detail=f"Invalid role: {user_data.role}"
                    )
                update_data["role"] = user_data.role
                
            if user_data.password is not None:
                # Hash new password
                update_data["hashed_password"] = get_password_hash(user_data.password)
                
            # Note: UserUpdate schema doesn't have is_active field
            # This field is handled separately through activate_user/deactivate_user methods
                
            # Add update timestamp
            update_data["updated_at"] = datetime.utcnow()
            
            # Update user
            await self.db.execute(
                update(User).where(User.id == user_id).values(**update_data)
            )
            await self.db.commit()
            
            # Get updated user
            updated_user = await self.get_user_by_id(user_id)
            logger.info(f"✅ User updated: {user.username}")
            return updated_user
            
        except HTTPException:
            raise
        except Exception as e:
            logger.error(f"❌ User update failed: {e}")
            await self.db.rollback()
            raise HTTPException(
                status_code=status.HTTP_500_INTERNAL_SERVER_ERROR,
                detail="User update failed"
            )
            
    async def delete_user(self, user_id: int) -> bool:
        """Delete user (soft delete)"""
        try:
            # Get existing user
            user = await self.get_user_by_id(user_id)
            if not user:
                raise HTTPException(
                    status_code=status.HTTP_404_NOT_FOUND,
                    detail="User not found"
                )
                
            # Soft delete - mark as inactive
            await self.db.execute(
                update(User).where(User.id == user_id).values(
                    is_active=False,
                    deleted_at=datetime.utcnow()
                )
            )
            await self.db.commit()
            
            logger.info(f"✅ User deleted (soft): {user.username}")
            return True
            
        except HTTPException:
            raise
        except Exception as e:
            logger.error(f"❌ User deletion failed: {e}")
            await self.db.rollback()
            raise HTTPException(
                status_code=status.HTTP_500_INTERNAL_SERVER_ERROR,
                detail="User deletion failed"
            )
            
    async def assign_role(self, user_id: int, role: str) -> bool:
        """Assign role to user"""
        try:
            # Validate role
            if not self._is_valid_role(role):
                raise HTTPException(
                    status_code=status.HTTP_400_BAD_REQUEST,
                    detail=f"Invalid role: {role}"
                )
                
            # Update user role
            await self.db.execute(
                update(User).where(User.id == user_id).values(
                    role=role,
                    updated_at=datetime.utcnow()
                )
            )
            await self.db.commit()
            
            logger.info(f"✅ Role {role} assigned to user {user_id}")
            return True
            
        except HTTPException:
            raise
        except Exception as e:
            logger.error(f"❌ Role assignment failed: {e}")
            await self.db.rollback()
            raise HTTPException(
                status_code=status.HTTP_500_INTERNAL_SERVER_ERROR,
                detail="Role assignment failed"
            )
            
    async def change_password(self, user_id: int, old_password: str, new_password: str) -> bool:
        """Change user password"""
        try:
            # Get user
            user = await self.get_user_by_id(user_id)
            if not user:
                raise HTTPException(
                    status_code=status.HTTP_404_NOT_FOUND,
                    detail="User not found"
                )
                
            # Verify old password
            if not verify_password(old_password, user.hashed_password):
                raise HTTPException(
                    status_code=status.HTTP_400_BAD_REQUEST,
                    detail="Invalid old password"
                )
                
            # Hash new password
            hashed_new_password = get_password_hash(new_password)
            
            # Update password
            await self.db.execute(
                update(User).where(User.id == user_id).values(
                    hashed_password=hashed_new_password,
                    updated_at=datetime.utcnow()
                )
            )
            await self.db.commit()
            
            logger.info(f"✅ Password changed for user {user.username}")
            return True
            
        except HTTPException:
            raise
        except Exception as e:
            logger.error(f"❌ Password change failed: {e}")
            await self.db.rollback()
            raise HTTPException(
                status_code=status.HTTP_500_INTERNAL_SERVER_ERROR,
                detail="Password change failed"
            )
            
    async def activate_user(self, user_id: int) -> bool:
        """Activate user account"""
        try:
            await self.db.execute(
                update(User).where(User.id == user_id).values(
                    is_active=True,
                    updated_at=datetime.utcnow()
                )
            )
            await self.db.commit()
            
            logger.info(f"✅ User {user_id} activated")
            return True
            
        except Exception as e:
            logger.error(f"❌ User activation failed: {e}")
            await self.db.rollback()
            return False
            
    async def deactivate_user(self, user_id: int) -> bool:
        """Deactivate user account"""
        try:
            await self.db.execute(
                update(User).where(User.id == user_id).values(
                    is_active=False,
                    updated_at=datetime.utcnow()
                )
            )
            await self.db.commit()
            
            logger.info(f"✅ User {user_id} deactivated")
            return True
            
        except Exception as e:
            logger.error(f"❌ User deactivation failed: {e}")
            await self.db.rollback()
            return False
            
    async def get_user_statistics(self) -> Dict[str, Any]:
        """Get user management statistics"""
        try:
            # Get total users
            total_users_result = await self.db.execute(select(User))
            total_users = len(total_users_result.scalars().all())
            
            # Get active users
            active_users_result = await self.db.execute(
                select(User).where(User.is_active == True)
            )
            active_users = len(active_users_result.scalars().all())
            
            # Get users by role
            role_counts = {}
            for role in ["admin", "operator", "viewer", "guest"]:
                role_users_result = await self.db.execute(
                    select(User).where(User.role == role)
                )
                role_counts[role] = len(role_users_result.scalars().all())
                
            return {
                "total_users": total_users,
                "active_users": active_users,
                "inactive_users": total_users - active_users,
                "users_by_role": role_counts,
                "last_updated": datetime.utcnow().isoformat()
            }
            
        except Exception as e:
            logger.error(f"❌ User statistics failed: {e}")
            return {"error": str(e)}
            
    def _is_valid_role(self, role: str) -> bool:
        """Check if role is valid"""
        valid_roles = ["admin", "operator", "viewer", "guest"]
        return role in valid_roles
        
    async def validate_user_data(self, user_data: UserCreate) -> Dict[str, Any]:
        """Validate user data before creation"""
        errors = []
        warnings = []
        
        # Username validation
        if len(user_data.username) < 3:
            errors.append("Username must be at least 3 characters")
        if len(user_data.username) > 50:
            errors.append("Username must be less than 50 characters")
            
        # Email validation
        if "@" not in user_data.email:
            errors.append("Invalid email format")
            
        # Password validation
        if len(user_data.password) < 8:
            errors.append("Password must be at least 8 characters")
        if not any(c.isupper() for c in user_data.password):
            warnings.append("Password should contain uppercase letters")
        if not any(c.islower() for c in user_data.password):
            warnings.append("Password should contain lowercase letters")
        if not any(c.isdigit() for c in user_data.password):
            warnings.append("Password should contain numbers")
            
        # Role validation
        if not self._is_valid_role(user_data.role):
            errors.append(f"Invalid role: {user_data.role}")
            
        return {
            "valid": len(errors) == 0,
            "errors": errors,
            "warnings": warnings
        }
