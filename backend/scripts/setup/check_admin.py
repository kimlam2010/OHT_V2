#!/usr/bin/env python3
"""Check admin user and create token"""

import asyncio
from app.core.database import get_db_context
from app.core.security import verify_password, create_access_token
from sqlalchemy import text

async def check_admin():
    """Check admin user and create token"""
    try:
        async with get_db_context() as db:
            # Get admin user
            result = await db.execute(text("SELECT * FROM users WHERE username = 'admin'"))
            admin = result.fetchone()
            
            if admin:
                print(f"Admin user found: {admin[1]} (ID: {admin[0]})")
                
                # Test password verification
                test_password = "admin123"
                if verify_password(test_password, admin[2]):
                    print(f"✅ Password '{test_password}' is correct")
                    
                    # Create access token
                    token = create_access_token({"sub": str(admin[0])})
                    print(f"✅ Access token created: {token[:50]}...")
                    
                    # Test token format
                    headers = {"Authorization": f"Bearer {token}"}
                    print(f"✅ Auth headers: {headers}")
                    
                else:
                    print(f"❌ Password '{test_password}' is incorrect")
                    
                    # Try to find correct password
                    print("Trying to find correct password...")
                    for i in range(1, 10):
                        test_pass = f"admin{i}"
                        if verify_password(test_pass, admin[2]):
                            print(f"✅ Correct password found: '{test_pass}'")
                            break
                    else:
                        print("❌ No common password found")
                        
            else:
                print("❌ No admin user found")
                
    except Exception as e:
        print(f"Error checking admin: {e}")

if __name__ == "__main__":
    asyncio.run(check_admin())
