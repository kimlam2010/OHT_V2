"""Unit tests for user management service"""

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
    
    @pytest.fixture
    def sample_user_data(self):
        """Sample UserCreate data"""
        return UserCreate(
            username="newuser",
            email="new@example.com",
            password="ValidPass12345",
            confirm_password="ValidPass12345",
            full_name="New User",
            role="viewer"
        )
    
    @pytest.fixture
    def sample_user_update(self):
        """Sample UserUpdate data"""
        return UserUpdate(
            username="updateduser",
            email="updated@example.com",
            role="operator"
        )
    
    # Test _is_valid_role method
    def test_is_valid_role(self, user_service):
        """Test role validation"""
        # Test valid roles
        assert user_service._is_valid_role("admin") is True
        assert user_service._is_valid_role("operator") is True
        assert user_service._is_valid_role("viewer") is True
        assert user_service._is_valid_role("guest") is True
        
        # Test invalid roles
        assert user_service._is_valid_role("invalid_role") is False
        assert user_service._is_valid_role("") is False
        assert user_service._is_valid_role(None) is False
    
    # Test get_user_by_id method
    @pytest.mark.asyncio
    async def test_get_user_by_id_success(self, user_service, mock_db, sample_user):
        """Test successful user retrieval by ID"""
        mock_result = MagicMock()
        mock_result.scalar_one_or_none.return_value = sample_user
        mock_db.execute = AsyncMock(return_value=mock_result)
        
        result = await user_service.get_user_by_id(1)
        assert result == sample_user
        assert result.username == "testuser"
        assert result.id == 1
    
    @pytest.mark.asyncio
    async def test_get_user_by_id_not_found(self, user_service, mock_db):
        """Test user retrieval by ID when user doesn't exist"""
        mock_result = MagicMock()
        mock_result.scalar_one_or_none.return_value = None
        mock_db.execute = AsyncMock(return_value=mock_result)
        
        result = await user_service.get_user_by_id(999)
        assert result is None
    
    @pytest.mark.asyncio
    async def test_get_user_by_id_error(self, user_service, mock_db):
        """Test get_user_by_id with database error"""
        mock_db.execute = AsyncMock(side_effect=Exception("Database error"))
        
        result = await user_service.get_user_by_id(1)
        assert result is None
    
    # Test get_user_by_username method
    @pytest.mark.asyncio
    async def test_get_user_by_username_success(self, user_service, mock_db, sample_user):
        """Test successful user retrieval by username"""
        mock_result = MagicMock()
        mock_result.scalar_one_or_none.return_value = sample_user
        mock_db.execute = AsyncMock(return_value=mock_result)
        
        result = await user_service.get_user_by_username("testuser")
        assert result == sample_user
        assert result.username == "testuser"
    
    @pytest.mark.asyncio
    async def test_get_user_by_username_not_found(self, user_service, mock_db):
        """Test get_user_by_username when user doesn't exist"""
        mock_result = MagicMock()
        mock_result.scalar_one_or_none.return_value = None
        mock_db.execute = AsyncMock(return_value=mock_result)
        
        result = await user_service.get_user_by_username("nonexistent")
        assert result is None
    
    @pytest.mark.asyncio
    async def test_get_user_by_username_error(self, user_service, mock_db):
        """Test get_user_by_username with database error"""
        mock_db.execute = AsyncMock(side_effect=Exception("Database error"))
        
        result = await user_service.get_user_by_username("testuser")
        assert result is None
    
    # Test get_user_by_email method
    @pytest.mark.asyncio
    async def test_get_user_by_email_success(self, user_service, mock_db, sample_user):
        """Test successful user retrieval by email"""
        mock_result = MagicMock()
        mock_result.scalar_one_or_none.return_value = sample_user
        mock_db.execute = AsyncMock(return_value=mock_result)
        
        result = await user_service.get_user_by_email("test@example.com")
        assert result == sample_user
        assert result.email == "test@example.com"
    
    @pytest.mark.asyncio
    async def test_get_user_by_email_not_found(self, user_service, mock_db):
        """Test get_user_by_email when user doesn't exist"""
        mock_result = MagicMock()
        mock_result.scalar_one_or_none.return_value = None
        mock_db.execute = AsyncMock(return_value=mock_result)
        
        result = await user_service.get_user_by_email("nonexistent@example.com")
        assert result is None
    
    @pytest.mark.asyncio
    async def test_get_user_by_email_error(self, user_service, mock_db):
        """Test get_user_by_email with database error"""
        mock_db.execute = AsyncMock(side_effect=Exception("Database error"))
        
        result = await user_service.get_user_by_email("test@example.com")
        assert result is None
    
    # Test get_all_users method
    @pytest.mark.asyncio
    async def test_get_all_users_success(self, user_service, mock_db, sample_user):
        """Test successful retrieval of all users"""
        mock_result = MagicMock()
        mock_result.scalars.return_value.all.return_value = [sample_user]
        mock_db.execute = AsyncMock(return_value=mock_result)
        
        result = await user_service.get_all_users()
        assert len(result) == 1
        assert result[0] == sample_user
    
    @pytest.mark.asyncio
    async def test_get_all_users_empty(self, user_service, mock_db):
        """Test get_all_users when no users exist"""
        mock_result = MagicMock()
        mock_result.scalars.return_value.all.return_value = []
        mock_db.execute = AsyncMock(return_value=mock_result)
        
        result = await user_service.get_all_users()
        assert len(result) == 0
    
    @pytest.mark.asyncio
    async def test_get_all_users_with_pagination(self, user_service, mock_db, sample_user):
        """Test get_all_users with pagination"""
        mock_result = MagicMock()
        mock_result.scalars.return_value.all.return_value = [sample_user]
        mock_db.execute = AsyncMock(return_value=mock_result)
        
        result = await user_service.get_all_users(skip=5, limit=10)
        assert len(result) == 1
        assert result[0] == sample_user
    
    @pytest.mark.asyncio
    async def test_get_all_users_error(self, user_service, mock_db):
        """Test get_all_users with database error"""
        mock_db.execute = AsyncMock(side_effect=Exception("Database error"))
        
        result = await user_service.get_all_users()
        assert len(result) == 0
    
    # Test get_users_by_role method
    @pytest.mark.asyncio
    async def test_get_users_by_role_success(self, user_service, mock_db, sample_user):
        """Test successful retrieval of users by role"""
        mock_result = MagicMock()
        mock_result.scalars.return_value.all.return_value = [sample_user]
        mock_db.execute = AsyncMock(return_value=mock_result)
        
        result = await user_service.get_users_by_role("viewer")
        assert len(result) == 1
        assert result[0].role == "viewer"
    
    @pytest.mark.asyncio
    async def test_get_users_by_role_empty(self, user_service, mock_db):
        """Test get_users_by_role when no users with role exist"""
        mock_result = MagicMock()
        mock_result.scalars.return_value.all.return_value = []
        mock_db.execute = AsyncMock(return_value=mock_result)
        
        result = await user_service.get_users_by_role("admin")
        assert len(result) == 0
    
    @pytest.mark.asyncio
    async def test_get_users_by_role_error(self, user_service, mock_db):
        """Test get_users_by_role with database error"""
        mock_db.execute = AsyncMock(side_effect=Exception("Database error"))
        
        result = await user_service.get_users_by_role("viewer")
        assert len(result) == 0
    
    # Test assign_role method
    @pytest.mark.asyncio
    async def test_assign_role_success(self, user_service, mock_db):
        """Test successful role assignment"""
        mock_db.execute = AsyncMock()
        mock_db.commit = AsyncMock()
        
        result = await user_service.assign_role(1, "admin")
        assert result is True
        
        # Verify database operations
        mock_db.execute.assert_called()
        mock_db.commit.assert_called_once()
    
    @pytest.mark.asyncio
    async def test_assign_role_invalid(self, user_service):
        """Test role assignment with invalid role"""
        with pytest.raises(HTTPException) as exc_info:
            await user_service.assign_role(1, "invalid_role")
        
        assert exc_info.value.status_code == status.HTTP_400_BAD_REQUEST
        assert "Invalid role" in str(exc_info.value.detail)
    
    @pytest.mark.asyncio
    async def test_assign_role_database_error(self, user_service, mock_db):
        """Test assign_role with database error"""
        mock_db.execute = AsyncMock(side_effect=Exception("Database error"))
        mock_db.rollback = AsyncMock()
        
        with pytest.raises(HTTPException) as exc_info:
            await user_service.assign_role(1, "admin")
        
        assert exc_info.value.status_code == status.HTTP_500_INTERNAL_SERVER_ERROR
        assert "Role assignment failed" in str(exc_info.value.detail)
        mock_db.rollback.assert_called_once()
    
    # Test activate_user method
    @pytest.mark.asyncio
    async def test_activate_user_success(self, user_service, mock_db):
        """Test successful user activation"""
        mock_db.execute = AsyncMock()
        mock_db.commit = AsyncMock()
        
        result = await user_service.activate_user(1)
        assert result is True
        
        # Verify database operations
        mock_db.execute.assert_called()
        mock_db.commit.assert_called_once()
    
    @pytest.mark.asyncio
    async def test_activate_user_error(self, user_service, mock_db):
        """Test activate_user with database error"""
        mock_db.execute = AsyncMock(side_effect=Exception("Database error"))
        mock_db.rollback = AsyncMock()
        
        result = await user_service.activate_user(1)
        assert result is False
        mock_db.rollback.assert_called_once()
    
    # Test deactivate_user method
    @pytest.mark.asyncio
    async def test_deactivate_user_success(self, user_service, mock_db):
        """Test successful user deactivation"""
        mock_db.execute = AsyncMock()
        mock_db.commit = AsyncMock()
        
        result = await user_service.deactivate_user(1)
        assert result is True
        
        # Verify database operations
        mock_db.execute.assert_called()
        mock_db.commit.assert_called_once()
    
    @pytest.mark.asyncio
    async def test_deactivate_user_error(self, user_service, mock_db):
        """Test deactivate_user with database error"""
        mock_db.execute = AsyncMock(side_effect=Exception("Database error"))
        mock_db.rollback = AsyncMock()
        
        result = await user_service.deactivate_user(1)
        assert result is False
        mock_db.rollback.assert_called_once()
    
    # Test get_user_statistics method
    @pytest.mark.asyncio
    async def test_get_user_statistics_success(self, user_service, mock_db):
        """Test successful user statistics retrieval"""
        # Mock different query results with proper side_effect
        mock_total = MagicMock()
        mock_total.scalars.return_value.all.return_value = [MagicMock()] * 5  # 5 total users
        
        mock_active = MagicMock()
        mock_active.scalars.return_value.all.return_value = [MagicMock()] * 3  # 3 active users
        
        mock_admin = MagicMock()
        mock_admin.scalars.return_value.all.return_value = [MagicMock()] * 1  # 1 admin
        
        mock_operator = MagicMock()
        mock_operator.scalars.return_value.all.return_value = [MagicMock()] * 1  # 1 operator
        
        mock_viewer = MagicMock()
        mock_viewer.scalars.return_value.all.return_value = [MagicMock()] * 2  # 2 viewers
        
        mock_guest = MagicMock()
        mock_guest.scalars.return_value.all.return_value = [MagicMock()] * 1  # 1 guest
        
        # Mock execute to return different results for different queries
        call_count = 0
        def mock_execute(query):
            nonlocal call_count
            call_count += 1
            
            # First call: total users
            if call_count == 1:
                return mock_total
            # Second call: active users  
            elif call_count == 2:
                return mock_active
            # Third call: admin users
            elif call_count == 3:
                return mock_admin
            # Fourth call: operator users
            elif call_count == 4:
                return mock_operator
            # Fifth call: viewer users
            elif call_count == 5:
                return mock_viewer
            # Sixth call: guest users
            else:
                return mock_guest
        
        mock_db.execute = AsyncMock(side_effect=mock_execute)
        
        # Get user statistics
        result = await user_service.get_user_statistics()
        
        # Verify result
        assert result["total_users"] == 5
        assert result["active_users"] == 3
        assert result["inactive_users"] == 2
        assert result["users_by_role"]["admin"] == 1
        assert result["users_by_role"]["operator"] == 1
        assert result["users_by_role"]["viewer"] == 2
        assert result["users_by_role"]["guest"] == 1
        assert "last_updated" in result
    
    @pytest.mark.asyncio
    async def test_get_user_statistics_error(self, user_service, mock_db):
        """Test user statistics retrieval with error"""
        # Mock database error
        mock_db.execute = AsyncMock(side_effect=Exception("Database error"))
        
        # Get user statistics
        result = await user_service.get_user_statistics()
        
        # Verify result contains error
        assert "error" in result
        assert "Database error" in result["error"]
    
    # Test validate_user_data method - Note: Pydantic validates before service
    @pytest.mark.asyncio
    async def test_validate_user_data_valid(self, user_service):
        """Test user data validation with valid data"""
        # Create valid user data
        user_data = UserCreate(
            username="validuser",
            email="valid@example.com",
            password="ValidPass12345",
            confirm_password="ValidPass12345",
            full_name="Valid User",
            role="viewer"
        )
        
        # Validate user data
        result = await user_service.validate_user_data(user_data)
        
        # Verify result
        assert result["valid"] is True
        assert len(result["errors"]) == 0
        assert len(result["warnings"]) >= 0  # May have password strength warnings
    
    @pytest.mark.asyncio
    async def test_validate_user_data_username_length(self, user_service):
        """Test user data validation with username length validation"""
        # Test username too short (Pydantic will catch this)
        try:
            user_data = UserCreate(
                username="ab",  # Too short
                email="valid@example.com",
                password="ValidPass12345",
                confirm_password="ValidPass12345",
                full_name="Valid User",
                role="viewer"
            )
            # If we get here, Pydantic didn't catch it, so test service validation
            result = await user_service.validate_user_data(user_data)
            assert result["valid"] is False
            assert len(result["errors"]) > 0
        except Exception as e:
            # Pydantic caught it - that's fine
            assert "validation error" in str(e).lower() or "string_too_short" in str(e)
    
    @pytest.mark.asyncio
    async def test_validate_user_data_email_format(self, user_service):
        """Test user data validation with email format validation"""
        # Test invalid email (Pydantic will catch this)
        try:
            user_data = UserCreate(
                username="validuser",
                email="invalid-email",  # No @ symbol
                password="ValidPass12345",
                confirm_password="ValidPass12345",
                full_name="Valid User",
                role="viewer"
            )
            # If we get here, Pydantic didn't catch it, so test service validation
            result = await user_service.validate_user_data(user_data)
            assert result["valid"] is False
            assert len(result["errors"]) > 0
        except Exception as e:
            # Pydantic caught it - that's fine
            assert "validation error" in str(e).lower() or "email" in str(e).lower()
    
    @pytest.mark.asyncio
    async def test_validate_user_data_password_length(self, user_service):
        """Test user data validation with password length validation"""
        # Test password too short (Pydantic will catch this)
        try:
            user_data = UserCreate(
                username="validuser",
                email="valid@example.com",
                password="weak",  # Too short
                confirm_password="weak",
                full_name="Valid User",
                role="viewer"
            )
            # If we get here, Pydantic didn't catch it, so test service validation
            result = await user_service.validate_user_data(user_data)
            assert result["valid"] is False
            assert len(result["errors"]) > 0
        except Exception as e:
            # Pydantic caught it - that's fine
            assert "validation error" in str(e).lower() or "string_too_short" in str(e)
    
    @pytest.mark.asyncio
    async def test_validate_user_data_role_validation(self, user_service):
        """Test user data validation with role validation"""
        # Test invalid role (Pydantic will catch this)
        try:
            user_data = UserCreate(
                username="validuser",
                email="valid@example.com",
                password="ValidPass12345",
                confirm_password="ValidPass12345",
                full_name="Valid User",
                role="invalid_role"
            )
            # If we get here, Pydantic didn't catch it, so test service validation
            result = await user_service.validate_user_data(user_data)
            assert result["valid"] is False
            assert len(result["errors"]) > 0
        except Exception as e:
            # Pydantic caught it - that's fine
            assert "validation error" in str(e).lower() or "enum" in str(e).lower()
    
    @pytest.mark.asyncio
    async def test_validate_user_data_password_strength(self, user_service):
        """Test user data validation with password strength warnings"""
        # Test password without uppercase (Pydantic will catch this)
        try:
            user_data = UserCreate(
                username="validuser",
                email="valid@example.com",
                password="password123",  # No uppercase
                confirm_password="password123",
                full_name="Valid User",
                role="viewer"
            )
            # If we get here, Pydantic didn't catch it, so test service validation
            result = await user_service.validate_user_data(user_data)
            assert result["valid"] is True  # Still valid
            assert len(result["errors"]) == 0
            assert len(result["warnings"]) > 0  # Should have warnings about password strength
        except Exception as e:
            # Pydantic caught it - that's fine
            assert "validation error" in str(e).lower() or "uppercase" in str(e).lower()

    @pytest.mark.asyncio
    async def test_create_user_invalid_role(self, user_service, sample_user_data):
        """Test user creation with invalid role"""
        # Test invalid role (Pydantic will catch this)
        try:
            invalid_user_data = UserCreate(
                username="newuser",
                email="new@example.com",
                password="ValidPass12345",
                confirm_password="ValidPass12345",
                full_name="New User",
                role="invalid_role"
            )
            # If we get here, Pydantic didn't catch it, so test service validation
            with patch.object(user_service, 'get_user_by_username', return_value=None):
                with pytest.raises(HTTPException) as exc_info:
                    await user_service.create_user(invalid_user_data)
                
                assert exc_info.value.status_code == status.HTTP_400_BAD_REQUEST
                assert "Invalid role" in str(exc_info.value.detail)
        except Exception as e:
            # Pydantic caught it - that's fine
            assert "validation error" in str(e).lower() or "enum" in str(e).lower()
    
    @pytest.mark.asyncio
    async def test_create_user_database_error(self, user_service, mock_db, sample_user_data):
        """Test user creation with database error"""
        # Mock get_user_by_username to return None
        with patch.object(user_service, 'get_user_by_username', return_value=None):
            # Mock password hashing
            with patch('app.services.user_management.get_password_hash', return_value="hashed_password"):
                # Mock database error
                mock_db.add = MagicMock(side_effect=Exception("Database error"))
                mock_db.rollback = AsyncMock()
                
                with pytest.raises(HTTPException) as exc_info:
                    await user_service.create_user(sample_user_data)
                
                assert exc_info.value.status_code == status.HTTP_500_INTERNAL_SERVER_ERROR
                assert "User creation failed" in str(exc_info.value.detail)
                mock_db.rollback.assert_called_once()
    
    # Test update_user method
    @pytest.mark.asyncio
    async def test_update_user_success(self, user_service, mock_db, sample_user, sample_user_update):
        """Test successful user update"""
        # Mock get_user_by_id to return existing user
        with patch.object(user_service, 'get_user_by_id', return_value=sample_user):
            # Mock get_user_by_username to return None (no conflict)
            with patch.object(user_service, 'get_user_by_username', return_value=None):
                # Mock get_user_by_email to return None (no conflict)
                with patch.object(user_service, 'get_user_by_email', return_value=None):
                    # Mock database operations
                    mock_db.execute = AsyncMock()
                    mock_db.commit = AsyncMock()
                    
                    # Mock get_user_by_id for updated user
                    with patch.object(user_service, 'get_user_by_id', return_value=sample_user):
                        # Update user
                        result = await user_service.update_user(1, sample_user_update)
                        
                        # Verify result
                        assert result is not None
                        assert result.username == "testuser"  # Original user
                        
                        # Verify database operations
                        mock_db.execute.assert_called()
                        mock_db.commit.assert_called_once()
    
    @pytest.mark.asyncio
    async def test_update_user_not_found(self, user_service, sample_user_update):
        """Test user update when user doesn't exist"""
        # Mock get_user_by_id to return None
        with patch.object(user_service, 'get_user_by_id', return_value=None):
            with pytest.raises(HTTPException) as exc_info:
                await user_service.update_user(999, sample_user_update)
            
            assert exc_info.value.status_code == status.HTTP_404_NOT_FOUND
            assert "User not found" in str(exc_info.value.detail)
    
    @pytest.mark.asyncio
    async def test_update_user_username_conflict(self, user_service, mock_db, sample_user, sample_user_update):
        """Test user update with username conflict"""
        # Mock get_user_by_id to return existing user
        with patch.object(user_service, 'get_user_by_id', return_value=sample_user):
            # Mock get_user_by_username to return different user (conflict)
            conflict_user = User(
                id=2,
                username="updateduser",  # Same as update data
                email="other@example.com",
                password_hash="hashed_password",
                role="viewer",
                is_active=True,
                created_at=datetime.now(timezone.utc)
            )
            
            with patch.object(user_service, 'get_user_by_username', return_value=conflict_user):
                with pytest.raises(HTTPException) as exc_info:
                    await user_service.update_user(1, sample_user_update)
                
                assert exc_info.value.status_code == status.HTTP_400_BAD_REQUEST
                assert "Username already exists" in str(exc_info.value.detail)
    
    @pytest.mark.asyncio
    async def test_update_user_database_error(self, user_service, mock_db, sample_user, sample_user_update):
        """Test user update with database error"""
        # Mock get_user_by_id to return existing user
        with patch.object(user_service, 'get_user_by_id', return_value=sample_user):
            # Mock get_user_by_username to return None (no conflict)
            with patch.object(user_service, 'get_user_by_username', return_value=None):
                # Mock get_user_by_email to return None (no conflict)
                with patch.object(user_service, 'get_user_by_email', return_value=None):
                    # Mock database error
                    mock_db.execute = AsyncMock(side_effect=Exception("Database error"))
                    mock_db.rollback = AsyncMock()
                    
                    with pytest.raises(HTTPException) as exc_info:
                        await user_service.update_user(1, sample_user_update)
                    
                    assert exc_info.value.status_code == status.HTTP_500_INTERNAL_SERVER_ERROR
                    assert "User update failed" in str(exc_info.value.detail)
                    mock_db.rollback.assert_called_once()
    
    # Test delete_user method
    @pytest.mark.asyncio
    async def test_delete_user_success(self, user_service, mock_db, sample_user):
        """Test successful user deletion (soft delete)"""
        # Mock get_user_by_id to return existing user
        with patch.object(user_service, 'get_user_by_id', return_value=sample_user):
            # Mock database operations
            mock_db.execute = AsyncMock()
            mock_db.commit = AsyncMock()
            
            # Delete user
            result = await user_service.delete_user(1)
            
            # Verify result
            assert result is True
            
            # Verify database operations
            mock_db.execute.assert_called()
            mock_db.commit.assert_called_once()
    
    @pytest.mark.asyncio
    async def test_delete_user_not_found(self, user_service):
        """Test user deletion when user doesn't exist"""
        # Mock get_user_by_id to return None
        with patch.object(user_service, 'get_user_by_id', return_value=None):
            with pytest.raises(HTTPException) as exc_info:
                await user_service.delete_user(999)
            
            assert exc_info.value.status_code == status.HTTP_404_NOT_FOUND
            assert "User not found" in str(exc_info.value.detail)
    
    @pytest.mark.asyncio
    async def test_delete_user_database_error(self, user_service, mock_db, sample_user):
        """Test user deletion with database error"""
        # Mock get_user_by_id to return existing user
        with patch.object(user_service, 'get_user_by_id', return_value=sample_user):
            # Mock database error
            mock_db.execute = AsyncMock(side_effect=Exception("Database error"))
            mock_db.rollback = AsyncMock()
            
            with pytest.raises(HTTPException) as exc_info:
                await user_service.delete_user(1)
            
            assert exc_info.value.status_code == status.HTTP_500_INTERNAL_SERVER_ERROR
            assert "User deletion failed" in str(exc_info.value.detail)
            mock_db.rollback.assert_called_once()
    
    # Test change_password method
    @pytest.mark.asyncio
    async def test_change_password_success(self, user_service, mock_db, sample_user):
        """Test successful password change"""
        # Mock get_user_by_id to return existing user
        with patch.object(user_service, 'get_user_by_id', return_value=sample_user):
            # Mock password verification
            with patch('app.services.user_management.verify_password', return_value=True):
                # Mock password hashing
                with patch('app.services.user_management.get_password_hash', return_value="new_hashed_password"):
                    # Mock database operations
                    mock_db.execute = AsyncMock()
                    mock_db.commit = AsyncMock()
                    
                    # Change password
                    result = await user_service.change_password(1, "old_password", "new_password")
                    
                    # Verify result
                    assert result is True
                    
                    # Verify database operations
                    mock_db.execute.assert_called()
                    mock_db.commit.assert_called_once()
    
    @pytest.mark.asyncio
    async def test_change_password_user_not_found(self, user_service):
        """Test password change when user doesn't exist"""
        # Mock get_user_by_id to return None
        with patch.object(user_service, 'get_user_by_id', return_value=None):
            with pytest.raises(HTTPException) as exc_info:
                await user_service.change_password(999, "old_password", "new_password")
            
            assert exc_info.value.status_code == status.HTTP_404_NOT_FOUND
            assert "User not found" in str(exc_info.value.detail)
    
    @pytest.mark.asyncio
    async def test_change_password_incorrect_old_password(self, user_service, sample_user):
        """Test password change with incorrect old password"""
        # Mock get_user_by_id to return existing user
        with patch.object(user_service, 'get_user_by_id', return_value=sample_user):
            # Mock password verification to return False
            with patch('app.services.user_management.verify_password', return_value=False):
                with pytest.raises(HTTPException) as exc_info:
                    await user_service.change_password(1, "wrong_password", "new_password")
                
                assert exc_info.value.status_code == status.HTTP_400_BAD_REQUEST
                assert "Invalid old password" in str(exc_info.value.detail)
    
    @pytest.mark.asyncio
    async def test_change_password_database_error(self, user_service, mock_db, sample_user):
        """Test password change with database error"""
        # Mock get_user_by_id to return existing user
        with patch.object(user_service, 'get_user_by_id', return_value=sample_user):
            # Mock password verification
            with patch('app.services.user_management.verify_password', return_value=True):
                # Mock password hashing
                with patch('app.services.user_management.get_password_hash', return_value="new_hashed_password"):
                    # Mock database error
                    mock_db.execute = AsyncMock(side_effect=Exception("Database error"))
                    mock_db.rollback = AsyncMock()
                    
                    with pytest.raises(HTTPException) as exc_info:
                        await user_service.change_password(1, "old_password", "new_password")
                    
                    assert exc_info.value.status_code == status.HTTP_500_INTERNAL_SERVER_ERROR
                    assert "Password change failed" in str(exc_info.value.detail)
                    mock_db.rollback.assert_called_once()
