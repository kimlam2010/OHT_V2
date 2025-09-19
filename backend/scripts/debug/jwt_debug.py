#!/usr/bin/env python3
"""
JWT Debug Utility for OHT-50 Backend
Helps troubleshoot JWT token authentication issues
"""

import os
import sys
from datetime import datetime, timezone, timedelta
from jose import jwt, JWTError, ExpiredSignatureError
import json

# Add the backend app to path
sys.path.append(os.path.join(os.path.dirname(__file__), '..', '..'))

from app.config import settings
from app.core.security import create_access_token, verify_token, SECURITY_CONFIG

def print_header(title: str):
    """Print formatted header"""
    print("\n" + "="*60)
    print(f"🔍 {title}")
    print("="*60)

def print_info(key: str, value: str, status: str = "ℹ️"):
    """Print formatted info"""
    print(f"{status} {key}: {value}")

def print_success(message: str):
    """Print success message"""
    print(f"✅ {message}")

def print_error(message: str):
    """Print error message"""
    print(f"❌ {message}")

def print_warning(message: str):
    """Print warning message"""
    print(f"⚠️  {message}")

def check_jwt_config():
    """Check JWT configuration"""
    print_header("JWT Configuration Check")
    
    # Check settings
    print_info("JWT Secret", "***HIDDEN***" if settings.jwt_secret else "NOT SET", 
               "✅" if settings.jwt_secret else "❌")
    print_info("JWT Algorithm", settings.jwt_algorithm)
    print_info("JWT Expiry (config)", f"{settings.jwt_expiry} seconds")
    print_info("JWT Expiry (minutes)", f"{settings.jwt_expiry_minutes} minutes")
    
    # Check SECURITY_CONFIG
    print_info("Security Config JWT Secret", "***HIDDEN***" if SECURITY_CONFIG["jwt_secret"] else "NOT SET",
               "✅" if SECURITY_CONFIG["jwt_secret"] else "❌")
    print_info("Security Config JWT Expiry", f"{SECURITY_CONFIG['jwt_expiry']} seconds")
    
    # Check consistency
    if settings.jwt_expiry == SECURITY_CONFIG["jwt_expiry"]:
        print_success("JWT expiry configuration is consistent")
    else:
        print_error(f"JWT expiry mismatch: settings={settings.jwt_expiry}, security_config={SECURITY_CONFIG['jwt_expiry']}")
    
    # Check environment
    testing_mode = os.getenv("TESTING", "false").lower() == "true"
    print_info("Testing Mode", str(testing_mode))
    
    if not settings.jwt_secret and not testing_mode:
        print_error("JWT_SECRET not set for production mode!")
        return False
    
    return True

def create_test_token():
    """Create a test JWT token"""
    print_header("Create Test JWT Token")
    
    try:
        # Create test token
        test_data = {"sub": "1", "username": "admin"}
        token = create_access_token(data=test_data)
        
        print_success("Test token created successfully")
        print_info("Token", token[:50] + "...")
        
        # Decode token to show payload
        try:
            payload = jwt.decode(
                token,
                SECURITY_CONFIG["jwt_secret"],
                algorithms=[SECURITY_CONFIG["jwt_algorithm"]]
            )
            
            print_info("Token Payload", json.dumps(payload, indent=2, default=str))
            
            # Check expiry
            if "exp" in payload:
                exp_time = datetime.fromtimestamp(payload["exp"], tz=timezone.utc)
                now = datetime.now(timezone.utc)
                time_left = exp_time - now
                
                print_info("Expires At", exp_time.isoformat())
                print_info("Time Left", str(time_left))
                
                if time_left.total_seconds() > 0:
                    print_success(f"Token is valid for {time_left.total_seconds():.0f} seconds")
                else:
                    print_error("Token has already expired!")
            
        except Exception as e:
            print_error(f"Failed to decode token: {e}")
        
        return token
        
    except Exception as e:
        print_error(f"Failed to create test token: {e}")
        return None

def verify_test_token(token: str):
    """Verify a JWT token"""
    print_header("Verify JWT Token")
    
    if not token:
        print_error("No token provided")
        return False
    
    try:
        # Use our verify_token function
        payload = verify_token(token)
        
        if payload:
            print_success("Token verification successful")
            print_info("Payload", json.dumps(payload, indent=2, default=str))
            return True
        else:
            print_error("Token verification failed")
            return False
            
    except Exception as e:
        print_error(f"Token verification error: {e}")
        return False

def test_token_expiry():
    """Test token expiry behavior"""
    print_header("Token Expiry Test")
    
    try:
        # Create token with short expiry
        test_data = {"sub": "1", "username": "admin"}
        short_token = create_access_token(
            data=test_data, 
            expires_delta=timedelta(seconds=2)
        )
        
        print_info("Created token with 2-second expiry", short_token[:50] + "...")
        
        # Verify immediately
        if verify_token(short_token):
            print_success("Token valid immediately after creation")
        else:
            print_error("Token invalid immediately after creation")
        
        # Wait and verify again
        import time
        print_info("Waiting", "3 seconds...")
        time.sleep(3)
        
        if verify_token(short_token):
            print_error("Token still valid after expiry!")
        else:
            print_success("Token correctly expired")
            
    except Exception as e:
        print_error(f"Expiry test failed: {e}")

def main():
    """Main debug function"""
    print_header("OHT-50 JWT Debug Utility")
    print_info("Purpose", "Debug JWT token authentication issues")
    print_info("Issue", "GitHub Issue #104 - JWT Token Authentication")
    
    # Check configuration
    if not check_jwt_config():
        print_error("JWT configuration issues found!")
        return 1
    
    # Create and test token
    token = create_test_token()
    if token:
        verify_test_token(token)
    
    # Test expiry
    test_token_expiry()
    
    print_header("Debug Summary")
    print_success("JWT debug utility completed")
    print_info("Next Steps", "1. Fix any configuration issues found")
    print_info("", "2. Test with real authentication endpoints")
    print_info("", "3. Monitor JWT token expiry in production")
    
    return 0

if __name__ == "__main__":
    exit_code = main()
    sys.exit(exit_code)
