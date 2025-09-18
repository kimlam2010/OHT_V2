#!/usr/bin/env python3
"""
Get JWT token for API testing
"""

import sys
import os
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))

from app.core.security import create_access_token

# Create token for admin user
token = create_access_token(data={"sub": "1", "type": "access"})
print(f"JWT_TOKEN={token}")
