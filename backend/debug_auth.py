#!/usr/bin/env python3
"""Debug authentication in test environment"""

import asyncio
from app.core.database import get_db_context
from app.core.security import create_access_token
from sqlalchemy import text

async def debug_auth():
    """Debug authentication"""
    try:
        # Get admin user
        async with get_db_context() as db:
            result = await db.execute(text("SELECT id FROM users WHERE username = 'admin'"))
            admin = result.fetchone()
            
            if admin:
                user_id = admin[0]
                print(f"Admin user ID: {user_id}")
                
                # Create token
                token = create_access_token({"sub": str(user_id)})
                print(f"Token: {token}")
                
                # Test token format
                headers = {"Authorization": f"Bearer {token}"}
                print(f"Headers: {headers}")
                
                # Test token validation
                from app.core.security import verify_token
                try:
                    payload = verify_token(token)
                    print(f"Token payload: {payload}")
                    print(f"User ID from token: {payload.get('sub')}")
                except Exception as e:
                    print(f"Token validation error: {e}")
                    
            else:
                print("No admin user found")
                
    except Exception as e:
        print(f"Error: {e}")

if __name__ == "__main__":
    asyncio.run(debug_auth())
