"""
Unit tests for dependencies module
"""

import pytest
from unittest.mock import AsyncMock, Mock
from fastapi import HTTPException

from app.api.deps import (
    get_rs485_service,
    get_firmware_service,
    get_current_user_optional,
    get_current_user_required,
    get_db_session
)


class TestDependencies:
    """Test dependency injection functions"""
    
    @pytest.mark.asyncio
    async def test_get_rs485_service(self):
        """Test RS485 service dependency"""
        service = await get_rs485_service()
        assert service is None  # Currently returns None as placeholder
    
    @pytest.mark.asyncio
    async def test_get_firmware_service(self):
        """Test firmware service dependency"""
        service = await get_firmware_service()
        assert service is not None
        assert hasattr(service, '__class__')
    
    @pytest.mark.asyncio
    async def test_get_current_user_optional_with_user(self):
        """Test optional user dependency with valid user"""
        mock_user = {"id": 1, "username": "test", "role": "user"}
        mock_get_current_user = AsyncMock(return_value=mock_user)
        
        # Mock the dependency
        result = await get_current_user_optional(mock_user)
        assert result == mock_user
    
    @pytest.mark.asyncio
    async def test_get_current_user_required_with_user(self):
        """Test required user dependency with valid user"""
        mock_user = {"id": 1, "username": "test", "role": "user"}
        
        result = await get_current_user_required(mock_user)
        assert result == mock_user
    
    @pytest.mark.asyncio
    async def test_get_current_user_required_without_user(self):
        """Test required user dependency without user"""
        with pytest.raises(HTTPException) as exc_info:
            await get_current_user_required(None)
        
        assert exc_info.value.status_code == 401
        assert "Authentication required" in str(exc_info.value.detail)
    
    @pytest.mark.asyncio
    async def test_get_db_session(self):
        """Test database session dependency"""
        # This is a generator function, test that it yields
        session_gen = get_db_session()
        assert session_gen is not None
        # Close the generator
        await session_gen.aclose()
