#!/usr/bin/env python3
"""Test JWT token generation and validation"""

import asyncio
from app.core.security import create_access_token, verify_token
from app.core.database import get_db_context
from sqlalchemy import text

async def test_jwt():
    """Test JWT token generation and validation"""
    try:
        # Test token generation
        user_data = {"sub": "1"}
        token = create_access_token(user_data)
        print(f"Generated token: {token[:50]}...")
        
        # Test token verification
        payload = verify_token(token)
        print(f"Token payload: {payload}")
        
        # Test with real user from database
        async with get_db_context() as db:
            result = await db.execute(text('SELECT id, username, role FROM users WHERE username = "admin"'))
            admin_user = result.fetchone()
            
            if admin_user:
                print(f"Admin user: ID={admin_user.id}, Username={admin_user.username}, Role={admin_user.role}")
                
                # Generate token for admin user
                admin_token = create_access_token({"sub": str(admin_user.id)})
                print(f"Admin token: {admin_token[:50]}...")
                
                # Verify admin token
                admin_payload = verify_token(admin_token)
                print(f"Admin token payload: {admin_payload}")
            else:
                print("Admin user not found")
                
    except Exception as e:
        print(f"Error testing JWT: {e}")

if __name__ == "__main__":
    asyncio.run(test_jwt())
