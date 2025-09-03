"""
Test configuration for OHT-50 Backend
"""

import pytest
import asyncio
from fastapi.testclient import TestClient
from httpx import AsyncClient
from sqlalchemy.ext.asyncio import create_async_engine, AsyncSession
from sqlalchemy.orm import sessionmaker

from app.main import app
from app.core.database import Base, get_db_context
from app.core.security import create_access_token, get_password_hash
from app.models.user import User
from app.config import settings
from sqlalchemy import text


@pytest.fixture
async def test_app(test_db):
    """Get test application instance with test database"""
    print("🔧 Setting up test_app fixture")
    
    # Override database dependency for testing
    from app.core.database import get_db_context
    
    async def override_get_db():
        async_session = sessionmaker(
            test_db,
            class_=AsyncSession,
            expire_on_commit=False
        )
        session = async_session()
        try:
            yield session
        finally:
            await session.close()
    
    # Override authentication dependency for testing
    from app.core.security import get_current_user
    
    async def override_get_current_user():
        """Override authentication for testing"""
        print("🔧 Using override_get_current_user for testing")
        return User(
            id=1,
            username="admin",
            email="admin@test.com",
            role="administrator",
            is_active=True
        )
    
    # Clear existing overrides and set new ones
    app.dependency_overrides.clear()
    app.dependency_overrides[get_db_context] = override_get_db
    app.dependency_overrides[get_current_user] = override_get_current_user
    
    print("🔧 Dependency overrides set:")
    print(f"  - Database: {get_db_context}")
    print(f"  - Authentication: {get_current_user}")
    print(f"  - Overrides count: {len(app.dependency_overrides)}")
    
    # Create test admin user if not exists
    async with test_db.begin() as conn:
        # Check if admin user exists
        result = await conn.execute(
            text("SELECT id FROM users WHERE username = 'admin'")
        )
        admin_user = result.fetchone()
        
        if not admin_user:
            await conn.execute(
                text("""
                INSERT INTO users (username, email, password_hash, role, is_active)
                VALUES (:username, :email, :password_hash, :role, :is_active)
                """),
                {
                    "username": "admin",
                    "email": "admin@test.com",
                    "password_hash": get_password_hash("admin123"),
                    "role": "administrator",
                    "is_active": True
                }
            )
            print("Created test admin user: admin/admin123")
    
    print("🔧 test_app fixture setup complete")
    yield app
    
    # Cleanup
    print("🔧 Cleaning up test_app fixture")
    app.dependency_overrides.clear()


@pytest.fixture
def client(test_app):
    """Create test client"""
    return TestClient(test_app)


@pytest.fixture
async def async_client(test_app):
    """Create async test client"""
    print(f"🔧 Creating async client with test_app: {test_app}")
    print(f"🔧 test_app dependency_overrides: {test_app.dependency_overrides}")
    print(f"🔧 test_app dependency_overrides keys: {list(test_app.dependency_overrides.keys())}")
    return AsyncClient(app=test_app, base_url="http://test")


@pytest.fixture
def admin_user():
    """Create admin user fixture"""
    return {
        "id": 1,
        "username": "admin",
        "email": "admin@test.com",
        "role": "administrator",
        "is_active": True
    }


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
    
    session = async_session()
    try:
        yield session
    finally:
        await session.close()


@pytest.fixture(scope="session")
def event_loop():
    """Create an instance of the default event loop for the test session"""
    loop = asyncio.get_event_loop_policy().new_event_loop()
    yield loop
    loop.close()
