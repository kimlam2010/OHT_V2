"""
Unit tests for user management service
"""

import pytest
from unittest.mock import AsyncMock, MagicMock, patch
from datetime import datetime, timezone
from fastapi import HTTPException, status
from sqlalchemy.ext.asyncio import AsyncSession

from app.services.user_management import UserManagementService
from app.models.user import User
from app.schemas.user import UserCreate, UserUpdate


class TestUserManagementService:
    """Test UserManagementService class"""
    
    @pytest.fixture
    def mock_db(self):
        """Create mock database session"""
        return AsyncMock(spec=AsyncSession)
    
    @pytest.fixture
    def user_service(self, mock_db):
        """Create UserManagementService instance"""
        return UserManagementService(mock_db)
    
    @pytest.fixture
    def sample_user_data(self):
        """Sample user data for testing"""
        return {
            "username": "testuser",
            "email": "test@example.com",
            "password": "TestPass123",
            "confirm_password": "TestPass123",
            "full_name": "Test User",
            "role": "viewer"
        }
    
    @pytest.fixture
    def sample_user(self):
        """Sample User model instance"""
        return User(
            id=1,
            username="testuser",
            email="test@example.com",
            password_hash="hashed_password",
            role="viewer",
            is_active=True,
            created_at=datetime.now(timezone.utc)
        )
    
    @pytest.mark.asyncio
    async def test_get_user_by_id_success(self, user_service, mock_db, sample_user):
        """Test successful user retrieval by ID"""
        # Mock database query result
        mock_result = MagicMock()
        mock_result.scalar_one_or_none.return_value = sample_user
        mock_db.execute = AsyncMock(return_value=mock_result)
        
        # Get user by ID
        result = await user_service.get_user_by_id(1)
        
        # Verify result
        assert result == sample_user
        assert result.username == "testuser"
        assert result.id == 1
    
    @pytest.mark.asyncio
    async def test_get_user_by_id_not_found(self, user_service, mock_db):
        """Test user retrieval by ID when user doesn't exist"""
        # Mock database query result
        mock_result = MagicMock()
        mock_result.scalar_one_or_none.return_value = None
        mock_db.execute = AsyncMock(return_value=mock_result)
        
        # Get user by ID
        result = await user_service.get_user_by_id(999)
        
        # Verify result
        assert result is None
    
    @pytest.mark.asyncio
    async def test_get_user_by_username_success(self, user_service, mock_db, sample_user):
        """Test successful user retrieval by username"""
        # Mock database query result
        mock_result = MagicMock()
        mock_result.scalar_one_or_none.return_value = sample_user
        mock_db.execute = AsyncMock(return_value=mock_result)
        
        # Get user by username
        result = await user_service.get_user_by_username("testuser")
        
        # Verify result
        assert result == sample_user
        assert result.username == "testuser"
    
    @pytest.mark.asyncio
    async def test_get_user_by_email_success(self, user_service, mock_db, sample_user):
        """Test successful user retrieval by email"""
        # Mock database query result
        mock_result = MagicMock()
        mock_result.scalar_one_or_none.return_value = sample_user
        mock_db.execute = AsyncMock(return_value=mock_result)
        
        # Get user by email
        result = await user_service.get_user_by_email("test@example.com")
        
        # Verify result
        assert result == sample_user
        assert result.email == "test@example.com"
    
    def test_is_valid_role(self, user_service):
        """Test role validation"""
        # Test valid roles
        assert user_service._is_valid_role("admin") is True
        assert user_service._is_valid_role("operator") is True
        assert user_service._is_valid_role("viewer") is True
        
        # Test invalid roles
        assert user_service._is_valid_role("invalid_role") is False
        assert user_service._is_valid_role("") is False
        assert user_service._is_valid_role(None) is False
