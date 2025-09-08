#!/usr/bin/env python3
"""Test fixture setup"""

import asyncio
import pytest
from app.main import app
from app.core.security import get_current_user
from app.models.user import User

async def test_fixture_setup():
    """Test fixture setup"""
    try:
        print("🔧 Testing fixture setup")
        
        # Check if app has dependency_overrides
        print(f"App dependency_overrides: {app.dependency_overrides}")
        print(f"App dependency_overrides keys: {list(app.dependency_overrides.keys())}")
        
        # Try to set dependency override
        async def override_get_current_user():
            """Override authentication for testing"""
            print("🔧 Using override_get_current_user for testing")
            return User(
                id=1,
                username="admin",
                email="admin@test.com",
                role="administrator",
                is_active=True
            )
        
        # Set dependency override
        app.dependency_overrides[get_current_user] = override_get_current_user
        
        print(f"After setting override - App dependency_overrides: {app.dependency_overrides}")
        print(f"After setting override - App dependency_overrides keys: {list(app.dependency_overrides.keys())}")
        
        # Test if override is working
        if get_current_user in app.dependency_overrides:
            print("✅ Dependency override set successfully")
        else:
            print("❌ Dependency override not set")
            
    except Exception as e:
        print(f"Error in test: {e}")

if __name__ == "__main__":
    asyncio.run(test_fixture_setup())
