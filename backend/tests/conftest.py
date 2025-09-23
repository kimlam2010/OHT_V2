"""
Test configuration for OHT-50 Backend
"""

import pytest
import pytest_asyncio
import asyncio
import warnings
import os
from fastapi.testclient import TestClient
from httpx import AsyncClient
from sqlalchemy.ext.asyncio import create_async_engine, AsyncSession
from sqlalchemy.orm import sessionmaker

from app.main import app
from app.core.database import Base, get_db_context
from app.core.security import create_access_token, get_password_hash
from app.models.user import User
from app.models.sensors import SensorConfiguration, SensorReading, SensorStatus, SensorCalibration  # Import sensor models
from app.models.telemetry import SensorData, TelemetryCurrent, TelemetryHistory, TelemetryAlert  # Import telemetry models
from app.models.robot import Robot, RobotCommand, RobotConfiguration  # Import robot models
from app.models.safety import SafetyStatus, SafetyLog, SafetyAlert, SafetyZone  # Import safety models
from app.models.map import Map  # Import map models
from app.config import settings
from sqlalchemy import text

# Ensure TESTING=true for all tests
os.environ["TESTING"] = "true"


def pytest_configure(config):
    # Ensure TESTING=true for all tests
    os.environ["TESTING"] = "true"
    
    # Fail on warnings and on skipped tests per QAQC rules
    warnings.simplefilter("error")
    config.addinivalue_line("markers", "performance: mark performance tests")
    
    # Set default timeout for all tests to prevent hang
    config.addinivalue_line("addopts", "--timeout=300")  # 5 minutes timeout


def pytest_runtest_logreport(report):
    # Hard fail if any test is skipped (but allow xfail)
    if report.when == "call" and report.skipped and not hasattr(report, 'wasxfail'):
        raise AssertionError(f"SKIPPED test found: {report.nodeid}")

@pytest.fixture
def test_app():
    """Return application instance without global overrides."""
    return app


@pytest.fixture
def client(test_app):
    """Create test client"""
    return TestClient(test_app)


@pytest_asyncio.fixture
async def async_client(test_app, test_db):
    """Create async test client with DB override only for this client."""
    from app.core.database import get_db
    
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
    
    # apply DB override only for async client scope
    app.dependency_overrides[get_db] = override_get_db
    try:
        async with AsyncClient(app=test_app, base_url="http://testserver") as client:
            yield client
    finally:
        # remove only our DB override
        app.dependency_overrides.pop(get_db, None)


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
def test_db(request):
    """Create test database with all tables (sync fixture returning AsyncEngine)."""
    # Use in-memory SQLite for testing
    test_engine = create_async_engine(
        "sqlite+aiosqlite:///:memory:",
        echo=False
    )

    print("🔧 Creating test database with all tables...")

    async def _setup_db():
        async with test_engine.begin() as conn:
            # Create all tables
            await conn.run_sync(Base.metadata.create_all)

            # Verify tables were created
            result = await conn.execute(text("SELECT name FROM sqlite_master WHERE type='table'"))
            tables = [row[0] for row in result.fetchall()]
            print(f"🔧 Created tables: {sorted(tables)}")

            # Verify critical tables exist
            critical_tables = ['users', 'sensor_configurations', 'robots', 'telemetry_current']
            missing_tables = [table for table in critical_tables if table not in tables]
            if missing_tables:
                print(f"⚠️  Missing critical tables: {missing_tables}")
            else:
                print("✅ All critical tables created successfully")

    loop = asyncio.get_event_loop()
    loop.run_until_complete(_setup_db())

    def _teardown():
        async def _dispose():
            await test_engine.dispose()
        asyncio.get_event_loop().run_until_complete(_dispose())

    request.addfinalizer(_teardown)
    return test_engine


@pytest_asyncio.fixture
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
