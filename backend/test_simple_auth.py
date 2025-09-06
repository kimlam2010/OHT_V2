#!/usr/bin/env python3
"""Simple authentication test"""

import asyncio
from app.core.security import create_access_token, verify_token
from app.models.user import User

async def test_simple_auth():
    """Test simple authentication"""
    try:
        # Create a simple user
        test_user = User(
            id=1,
            username="test",
            email="test@test.com",
            role="administrator",
            is_active=True
        )
        
        print(f"Test user: {test_user.username}, Role: {test_user.role}")
        
        # Test token creation
        token_data = {"sub": str(test_user.id)}
        token = create_access_token(token_data)
        print(f"Token created: {token[:50]}...")
        
        # Test token verification
        payload = verify_token(token)
        print(f"Token payload: {payload}")
        
        # Test user role check
        if test_user.role == "administrator":
            print("✅ User has administrator role")
        else:
            print("❌ User does not have administrator role")
            
        # Test permission check
        resource = "robot"
        permission = "read"
        
        if test_user.role == "administrator":
            print(f"✅ Administrator can access {resource}:{permission}")
        else:
            print(f"❌ User cannot access {resource}:{permission}")
            
    except Exception as e:
        print(f"Error in test: {e}")

if __name__ == "__main__":
    asyncio.run(test_simple_auth())
