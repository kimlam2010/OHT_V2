"""
Unit tests for user schemas
"""

import pytest
from datetime import datetime
from pydantic import ValidationError

from app.schemas.user import (
    UserRole,
    UserStatus,
    UserBase,
    UserCreate,
    UserUpdate,
    UserResponse,
    UserLogin,
    UserPasswordChange,
    UserProfile
)


class TestUserEnums:
    """Test user enums"""
    
    def test_user_role_enum(self):
        """Test UserRole enum values"""
        assert UserRole.ADMIN == "admin"
        assert UserRole.OPERATOR == "operator"
        assert UserRole.VIEWER == "viewer"
    
    def test_user_status_enum(self):
        """Test UserStatus enum values"""
        assert UserStatus.ACTIVE == "active"
        assert UserStatus.INACTIVE == "inactive"
        assert UserStatus.SUSPENDED == "suspended"


class TestUserBase:
    """Test UserBase model"""
    
    def test_user_base_valid(self):
        """Test valid UserBase creation"""
        user_data = {
            "username": "testuser",
            "email": "test@example.com",
            "full_name": "Test User",
            "role": UserRole.VIEWER,
            "status": UserStatus.ACTIVE
        }
        user = UserBase(**user_data)
        assert user.username == "testuser"
        assert user.email == "test@example.com"
        assert user.full_name == "Test User"
        assert user.role == UserRole.VIEWER
        assert user.status == UserStatus.ACTIVE
    
    def test_user_base_defaults(self):
        """Test UserBase with default values"""
        user_data = {
            "username": "testuser",
            "email": "test@example.com",
            "full_name": "Test User"
        }
        user = UserBase(**user_data)
        assert user.role == UserRole.VIEWER
        assert user.status == UserStatus.ACTIVE


class TestUserCreate:
    """Test UserCreate model"""
    
    def test_user_create_valid(self):
        """Test valid UserCreate creation"""
        user_data = {
            "username": "testuser",
            "email": "test@example.com",
            "full_name": "Test User",
            "password": "TestPass123",
            "confirm_password": "TestPass123"
        }
        user = UserCreate(**user_data)
        assert user.username == "testuser"
        assert user.password == "TestPass123"
    
    def test_user_create_passwords_match(self):
        """Test password confirmation validation"""
        user_data = {
            "username": "testuser",
            "email": "test@example.com",
            "full_name": "Test User",
            "password": "TestPass123",
            "confirm_password": "DifferentPass123"
        }
        with pytest.raises(ValidationError) as exc_info:
            UserCreate(**user_data)
        assert "Passwords do not match" in str(exc_info.value)
    
    def test_user_create_password_strength_uppercase(self):
        """Test password strength validation - uppercase"""
        user_data = {
            "username": "testuser",
            "email": "test@example.com",
            "full_name": "Test User",
            "password": "testpass123",
            "confirm_password": "testpass123"
        }
        with pytest.raises(ValidationError) as exc_info:
            UserCreate(**user_data)
        assert "Password must contain at least one uppercase letter" in str(exc_info.value)
    
    def test_user_create_password_strength_lowercase(self):
        """Test password strength validation - lowercase"""
        user_data = {
            "username": "testuser",
            "email": "test@example.com",
            "full_name": "Test User",
            "password": "TESTPASS123",
            "confirm_password": "TESTPASS123"
        }
        with pytest.raises(ValidationError) as exc_info:
            UserCreate(**user_data)
        assert "Password must contain at least one lowercase letter" in str(exc_info.value)
    
    def test_user_create_password_strength_digit(self):
        """Test password strength validation - digit"""
        user_data = {
            "username": "testuser",
            "email": "test@example.com",
            "full_name": "Test User",
            "password": "TestPassABC",
            "confirm_password": "TestPassABC"
        }
        with pytest.raises(ValidationError) as exc_info:
            UserCreate(**user_data)
        assert "Password must contain at least one digit" in str(exc_info.value)


class TestUserUpdate:
    """Test UserUpdate model"""
    
    def test_user_update_partial(self):
        """Test partial UserUpdate"""
        user_data = {
            "username": "newusername",
            "email": "new@example.com"
        }
        user = UserUpdate(**user_data)
        assert user.username == "newusername"
        assert user.email == "new@example.com"
        assert user.full_name is None
        assert user.role is None
        assert user.status is None
    
    def test_user_update_empty(self):
        """Test empty UserUpdate"""
        user = UserUpdate()
        assert user.username is None
        assert user.email is None
        assert user.full_name is None


class TestUserResponse:
    """Test UserResponse model"""
    
    def test_user_response_valid(self):
        """Test valid UserResponse creation"""
        now = datetime.now()
        user_data = {
            "id": 1,
            "username": "testuser",
            "email": "test@example.com",
            "full_name": "Test User",
            "role": UserRole.VIEWER,
            "status": UserStatus.ACTIVE,
            "created_at": now,
            "updated_at": now,
            "last_login": None
        }
        user = UserResponse(**user_data)
        assert user.id == 1
        assert user.username == "testuser"
        assert user.created_at == now


class TestUserLogin:
    """Test UserLogin model"""
    
    def test_user_login_valid(self):
        """Test valid UserLogin creation"""
        login_data = {
            "username": "testuser",
            "password": "TestPass123"
        }
        login = UserLogin(**login_data)
        assert login.username == "testuser"
        assert login.password == "TestPass123"


class TestUserPasswordChange:
    """Test UserPasswordChange model"""
    
    def test_user_password_change_valid(self):
        """Test valid UserPasswordChange creation"""
        change_data = {
            "current_password": "OldPass123",
            "new_password": "NewPass123",
            "confirm_new_password": "NewPass123"
        }
        change = UserPasswordChange(**change_data)
        assert change.current_password == "OldPass123"
        assert change.new_password == "NewPass123"
    
    def test_user_password_change_mismatch(self):
        """Test password confirmation mismatch"""
        change_data = {
            "current_password": "OldPass123",
            "new_password": "NewPass123",
            "confirm_new_password": "DifferentPass123"
        }
        with pytest.raises(ValidationError) as exc_info:
            UserPasswordChange(**change_data)
        assert "New passwords do not match" in str(exc_info.value)


class TestUserProfile:
    """Test UserProfile model"""
    
    def test_user_profile_valid(self):
        """Test valid UserProfile creation"""
        now = datetime.now()
        profile_data = {
            "id": 1,
            "username": "testuser",
            "email": "test@example.com",
            "full_name": "Test User",
            "role": UserRole.VIEWER,
            "status": UserStatus.ACTIVE,
            "created_at": now,
            "last_login": None
        }
        profile = UserProfile(**profile_data)
        assert profile.id == 1
        assert profile.username == "testuser"
        assert profile.created_at == now
