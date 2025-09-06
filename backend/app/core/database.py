"""
Database configuration and connection management
"""

from typing import AsyncGenerator, Dict, Any
from sqlalchemy.ext.asyncio import AsyncSession, create_async_engine, async_sessionmaker
from sqlalchemy.pool import NullPool
from sqlalchemy.orm import declarative_base
from contextlib import asynccontextmanager

from app.config import settings

# Create async engine with optimized connection pooling
engine = create_async_engine(
    settings.database_url,
    echo=settings.debug,
    poolclass=NullPool,  # Use NullPool for async SQLite
)

# Create async session factory
AsyncSessionLocal = async_sessionmaker(
    engine,
    class_=AsyncSession,
    expire_on_commit=False,
    autoflush=False,
    autocommit=False,
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


@asynccontextmanager
async def get_db_context():
    """Context manager for database sessions"""
    async with AsyncSessionLocal() as session:
        try:
            yield session
        except Exception:
            await session.rollback()
            raise
        finally:
            await session.close()


async def init_db():
    """Initialize database with optimized settings"""
    try:
        # Test connection
        from sqlalchemy import text
        async with engine.begin() as conn:
            await conn.run_sync(lambda sync_conn: sync_conn.execute(text("SELECT 1")))
        
        print("Database connection established successfully")
        
        # Create tables if they don't exist
        # Base is defined in this file, no need to import
        async with engine.begin() as conn:
            await conn.run_sync(Base.metadata.create_all)
        
        print("Database tables created/verified successfully")
        
    except Exception as e:
        print(f"Database initialization failed: {e}")
        raise


async def close_db():
    """Close database connections properly"""
    try:
        await engine.dispose()
        print("Database connections closed successfully")
    except Exception as e:
        print(f"Error closing database connections: {e}")


# Health check function
async def check_db_health() -> Dict[str, Any]:
    """Check database health and connection pool status"""
    try:
        from sqlalchemy import text
        async with engine.begin() as conn:
            await conn.run_sync(lambda sync_conn: sync_conn.execute(text("SELECT 1")))
        
        # Get pool status (simplified to avoid type issues)
        pool_status = {
            "pool_size": getattr(engine.pool, 'size', lambda: 0)(),
            "checked_in": getattr(engine.pool, 'checkedin', lambda: 0)(),
            "checked_out": getattr(engine.pool, 'checkedout', lambda: 0)(),
            "overflow": getattr(engine.pool, 'overflow', lambda: 0)(),
        }
        
        return {
            "status": "healthy",
            "pool_status": pool_status
        }
    except Exception as e:
        return {
            "status": "unhealthy",
            "error": str(e)
        }


async def create_test_admin_user():
    """Create test admin user for testing"""
    try:
        from sqlalchemy import text
        from app.core.security import get_password_hash
        
        async with get_db_context() as db:
            # Check if admin user exists
            result = await db.execute(
                text("SELECT id FROM users WHERE username = 'admin'")
            )
            admin_user = result.fetchone()
            
            if not admin_user:
                # Create admin user
                await db.execute(
                    text("""
                    INSERT INTO users (username, email, password_hash, role, is_active)
                    VALUES (:username, :email, :password_hash, :role, :is_active)
                    """),
                    {
                        "username": "admin",
                        "email": "admin@test.com",
                        "password_hash": get_password_hash("admin123"),
                        "role": "admin",
                        "is_active": True
                    }
                )
                await db.commit()
                print("Created test admin user: admin/admin123")
            else:
                print("Test admin user already exists")
                
    except Exception as e:
        print(f"Failed to create test admin user: {e}")
