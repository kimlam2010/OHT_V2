#!/usr/bin/env python3
"""Check users in database"""

import asyncio
from app.core.database import get_db_context
from sqlalchemy import text

async def check_users():
    """Check users in database"""
    try:
        async with get_db_context() as db:
            result = await db.execute(text('SELECT * FROM users'))
            users = result.fetchall()
            
            print(f'Found {len(users)} users:')
            for user in users:
                print(f'  ID: {user.id}, Username: {user.username}, Role: {user.role}, Active: {user.is_active}')
                
    except Exception as e:
        print(f"Error checking users: {e}")

if __name__ == "__main__":
    asyncio.run(check_users())
