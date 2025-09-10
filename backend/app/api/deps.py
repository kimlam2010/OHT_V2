"""
Dependencies injection for OHT-50 Backend
"""

from typing import Generator, AsyncGenerator
from fastapi import Depends, HTTPException, status
from sqlalchemy.ext.asyncio import AsyncSession

from app.core.database import get_db
from app.core.security import get_current_user
from app.services.firmware_integration_service import FirmwareIntegrationService


async def get_rs485_service():
    """Get RS485 service instance - placeholder for future implementation"""
    # TODO: Implement RS485 service when hardware is available
    return None


async def get_firmware_service() -> FirmwareIntegrationService:
    """Get firmware integration service instance"""
    return FirmwareIntegrationService()


async def get_current_user_optional(
    current_user: dict = Depends(get_current_user)
) -> dict:
    """Get current user (optional - for endpoints that can work without auth)"""
    return current_user


async def get_current_user_required(
    current_user: dict = Depends(get_current_user)
) -> dict:
    """Get current user (required - for protected endpoints)"""
    if not current_user:
        raise HTTPException(
            status_code=status.HTTP_401_UNAUTHORIZED,
            detail="Authentication required"
        )
    return current_user


async def get_db_session() -> AsyncGenerator[AsyncSession, None]:
    """Get database session"""
    async for session in get_db():
        yield session
