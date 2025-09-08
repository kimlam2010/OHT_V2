#!/usr/bin/env python3
"""Check database and users"""

import asyncio
from app.core.database import get_db_context
from sqlalchemy import text

async def check_database():
    """Check database contents"""
    try:
        async with get_db_context() as db:
            # Check users table
            result = await db.execute(text("SELECT * FROM users"))
            users = result.fetchall()
            print(f"Found {len(users)} users:")
            for user in users:
                print(f"- {user}")
            
            # Check if admin user exists
            result = await db.execute(text("SELECT * FROM users WHERE username = 'admin'"))
            admin = result.fetchone()
            if admin:
                print(f"Admin user found: {admin}")
            else:
                print("No admin user found")
                
    except Exception as e:
        print(f"Error checking database: {e}")

if __name__ == "__main__":
    asyncio.run(check_database())
