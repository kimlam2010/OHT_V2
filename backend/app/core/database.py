"""
Database configuration for OHT-50 Backend
"""

import asyncio
from typing import AsyncGenerator
from sqlalchemy.ext.asyncio import create_async_engine, AsyncSession, async_sessionmaker
from sqlalchemy.orm import declarative_base
from sqlalchemy.pool import StaticPool, QueuePool
from sqlalchemy import event

from app.config import settings

# Create async engine with connection pooling
engine = create_async_engine(
    settings.database_url,
    echo=settings.debug,
    poolclass=QueuePool,
    pool_size=20,  # Number of connections to maintain
    max_overflow=80,  # Additional connections when pool is full
    pool_pre_ping=True,  # Validate connections before use
    pool_recycle=3600,  # Recycle connections every hour
    connect_args={"check_same_thread": False} if "sqlite" in settings.database_url else {}
)

# Create async session factory
AsyncSessionLocal = async_sessionmaker(
    engine,
    class_=AsyncSession,
    expire_on_commit=False,
    autocommit=False,
    autoflush=False
)

# Base class for models
Base = declarative_base()


async def get_db() -> AsyncGenerator[AsyncSession, None]:
    """Get database session with proper cleanup"""
    async with AsyncSessionLocal() as session:
        try:
            yield session
        except Exception:
            await session.rollback()
            raise
        finally:
            await session.close()


async def init_db() -> None:
    """Initialize database with all tables"""
    async with engine.begin() as conn:
        # Import all models to ensure they are registered
        from app.models import robot, telemetry, safety, user
        
        # Create all tables
        await conn.run_sync(Base.metadata.create_all)
        
        # Create indexes for performance
        await create_indexes(conn)


async def create_indexes(conn) -> None:
    """Create additional indexes for performance"""
    # Robot status indexes
    await conn.execute("""
        CREATE INDEX IF NOT EXISTS idx_robot_status_robot_id 
        ON robots(robot_id)
    """)
    await conn.execute("""
        CREATE INDEX IF NOT EXISTS idx_robot_status_updated 
        ON robots(updated_at)
    """)
    
    # Telemetry indexes
    await conn.execute("""
        CREATE INDEX IF NOT EXISTS idx_telemetry_current_robot_timestamp 
        ON telemetry_current(robot_id, timestamp)
    """)
    await conn.execute("""
        CREATE INDEX IF NOT EXISTS idx_telemetry_history_robot_timestamp 
        ON telemetry_history(robot_id, timestamp)
    """)
    
    # Safety indexes
    await conn.execute("""
        CREATE INDEX IF NOT EXISTS idx_safety_logs_robot_timestamp 
        ON safety_logs(robot_id, timestamp)
    """)
    await conn.execute("""
        CREATE INDEX IF NOT EXISTS idx_safety_alerts_robot_status 
        ON safety_alerts(robot_id, status)
    """)
    
    # User indexes
    await conn.execute("""
        CREATE INDEX IF NOT EXISTS idx_user_sessions_token 
        ON user_sessions(session_token)
    """)
    await conn.execute("""
        CREATE INDEX IF NOT EXISTS idx_user_sessions_expires 
        ON user_sessions(expires_at)
    """)


async def close_db() -> None:
    """Close database connections"""
    await engine.dispose()


# Database health check
async def check_db_health() -> bool:
    """Check database connectivity"""
    try:
        async with AsyncSessionLocal() as session:
            await session.execute("SELECT 1")
            return True
    except Exception:
        return False
