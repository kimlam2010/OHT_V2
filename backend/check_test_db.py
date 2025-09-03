#!/usr/bin/env python3
"""Check test database with TESTING environment"""

import os
import asyncio
from app.core.database import get_db_context
from sqlalchemy import text

async def check_test_db():
    """Check test database"""
    # Set testing environment
    os.environ['TESTING'] = 'true'
    print(f"TESTING environment: {os.getenv('TESTING')}")
    
    try:
        async with get_db_context() as db:
            # Check users count
            result = await db.execute(text("SELECT COUNT(*) FROM users"))
            count = result.fetchone()[0]
            print(f"Test DB users count: {count}")
            
            # Check admin user
            result = await db.execute(text("SELECT id, username FROM users WHERE username = 'admin'"))
            admin = result.fetchone()
            if admin:
                print(f"Admin user found: ID={admin[0]}, username={admin[1]}")
            else:
                print("No admin user found in test DB")
                
    except Exception as e:
        print(f"Error checking test DB: {e}")

if __name__ == "__main__":
    asyncio.run(check_test_db())
