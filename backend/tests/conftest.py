"""
Test configuration for OHT-50 Backend
"""

import pytest
import asyncio
from fastapi.testclient import TestClient
from httpx import AsyncClient
from sqlalchemy.ext.asyncio import create_async_engine, AsyncSession
from sqlalchemy.orm import sessionmaker

from app.main import create_app
from app.core.database import Base, get_db_context
from app.core.security import create_access_token, get_password_hash
from app.config import settings


@pytest.fixture
def app():
    """Create test application"""
    return create_app()


@pytest.fixture
def client(app):
    """Create test client"""
    return TestClient(app)


@pytest.fixture
def async_client(app):
    """Create async test client"""
    return AsyncClient(app=app, base_url="http://test")


@pytest.fixture
async def test_db():
    """Create test database"""
    # Use in-memory SQLite for testing
    test_engine = create_async_engine(
        "sqlite+aiosqlite:///:memory:",
        echo=False
    )
    
    async with test_engine.begin() as conn:
        await conn.run_sync(Base.metadata.create_all)
    
    yield test_engine
    
    await test_engine.dispose()


@pytest.fixture
async def test_session(test_db):
    """Create test database session"""
    async_session = sessionmaker(
        test_db,
        class_=AsyncSession,
        expire_on_commit=False
    )
    
    async with async_session() as session:
        yield session


@pytest.fixture(scope="session")
def event_loop():
    """Create an instance of the default event loop for the test session"""
    loop = asyncio.get_event_loop_policy().new_event_loop()
    yield loop
    loop.close()
