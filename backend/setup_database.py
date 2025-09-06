#!/usr/bin/env python3
"""
Database setup script for OHT-50 Backend
"""

import asyncio
from app.core.database import init_db, AsyncSessionLocal
from app.core.security import create_default_roles, create_default_admin_user


async def setup_database():
    """Setup database with default data"""
    try:
        print("🔄 Initializing database...")
        await init_db()
        print("✅ Database initialized successfully")
        
        print("🔄 Creating default roles...")
        async with AsyncSessionLocal() as db:
            await create_default_roles(db)
            print("✅ Default roles created")
            
            print("🔄 Creating default admin user...")
            await create_default_admin_user(db)
            print("✅ Default admin user created")
            
        print("🎉 Database setup completed successfully!")
        print("📋 Default admin credentials: admin/admin123")
        
    except Exception as e:
        print(f"❌ Database setup failed: {e}")
        raise


if __name__ == "__main__":
    asyncio.run(setup_database())
