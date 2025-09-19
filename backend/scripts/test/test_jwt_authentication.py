#!/usr/bin/env python3
"""
JWT Authentication Test for OHT-50 Backend
Tests JWT token creation, validation, and expiry
"""

import os
import sys
import asyncio
import httpx
import json
from datetime import datetime, timezone

# Add the backend app to path
sys.path.append(os.path.join(os.path.dirname(__file__), '..', '..'))

BASE_URL = "http://127.0.0.1:8000"

def print_header(title: str):
    """Print formatted header"""
    print("\n" + "="*60)
    print(f"🧪 {title}")
    print("="*60)

def print_success(message: str):
    """Print success message"""
    print(f"✅ {message}")

def print_error(message: str):
    """Print error message"""
    print(f"❌ {message}")

def print_info(key: str, value: str):
    """Print info message"""
    print(f"ℹ️  {key}: {value}")

async def test_login():
    """Test login endpoint"""
    print_header("Login Test")
    
    async with httpx.AsyncClient() as client:
        try:
            response = await client.post(
                f"{BASE_URL}/api/v1/auth/login",
                json={
                    "username": "admin",
                    "password": "admin123"
                }
            )
            
            print_info("Status Code", str(response.status_code))
            
            if response.status_code == 200:
                data = response.json()
                print_success("Login successful")
                print_info("Token Type", data.get("token_type", "N/A"))
                print_info("Expires In", f"{data.get('expires_in', 'N/A')} seconds")
                
                access_token = data.get("access_token")
                if access_token:
                    print_info("Access Token", access_token[:50] + "...")
                    return access_token
                else:
                    print_error("No access token in response")
                    return None
            else:
                print_error(f"Login failed: {response.text}")
                return None
                
        except Exception as e:
            print_error(f"Login request failed: {e}")
            return None

async def test_authenticated_request(token: str):
    """Test authenticated request"""
    print_header("Authenticated Request Test")
    
    if not token:
        print_error("No token provided")
        return False
    
    async with httpx.AsyncClient() as client:
        try:
            headers = {"Authorization": f"Bearer {token}"}
            response = await client.get(
                f"{BASE_URL}/api/v1/auth/me",
                headers=headers
            )
            
            print_info("Status Code", str(response.status_code))
            
            if response.status_code == 200:
                data = response.json()
                print_success("Authenticated request successful")
                print_info("User ID", str(data.get("id", "N/A")))
                print_info("Username", data.get("username", "N/A"))
                print_info("Role", data.get("role", "N/A"))
                return True
            else:
                print_error(f"Authenticated request failed: {response.text}")
                return False
                
        except Exception as e:
            print_error(f"Authenticated request error: {e}")
            return False

async def test_token_refresh(token: str):
    """Test token refresh"""
    print_header("Token Refresh Test")
    
    # First get a refresh token
    async with httpx.AsyncClient() as client:
        try:
            # Login to get refresh token
            login_response = await client.post(
                f"{BASE_URL}/api/v1/auth/login",
                json={
                    "username": "admin",
                    "password": "admin123"
                }
            )
            
            if login_response.status_code != 200:
                print_error("Failed to get refresh token")
                return False
            
            login_data = login_response.json()
            refresh_token = login_data.get("refresh_token")
            
            if not refresh_token:
                print_error("No refresh token in login response")
                return False
            
            print_info("Refresh Token", refresh_token[:50] + "...")
            
            # Test refresh
            refresh_response = await client.post(
                f"{BASE_URL}/api/v1/auth/refresh",
                json={"refresh_token": refresh_token}
            )
            
            print_info("Status Code", str(refresh_response.status_code))
            
            if refresh_response.status_code == 200:
                refresh_data = refresh_response.json()
                print_success("Token refresh successful")
                print_info("New Token", refresh_data.get("access_token", "N/A")[:50] + "...")
                return True
            else:
                print_error(f"Token refresh failed: {refresh_response.text}")
                return False
                
        except Exception as e:
            print_error(f"Token refresh error: {e}")
            return False

async def test_invalid_token():
    """Test invalid token handling"""
    print_header("Invalid Token Test")
    
    async with httpx.AsyncClient() as client:
        try:
            # Test with invalid token
            headers = {"Authorization": "Bearer invalid-token-12345"}
            response = await client.get(
                f"{BASE_URL}/api/v1/auth/me",
                headers=headers
            )
            
            print_info("Status Code", str(response.status_code))
            
            if response.status_code == 401:
                print_success("Invalid token correctly rejected")
                return True
            else:
                print_error(f"Invalid token not rejected properly: {response.text}")
                return False
                
        except Exception as e:
            print_error(f"Invalid token test error: {e}")
            return False

async def test_no_token():
    """Test request without token"""
    print_header("No Token Test")
    
    async with httpx.AsyncClient() as client:
        try:
            response = await client.get(f"{BASE_URL}/api/v1/auth/me")
            
            print_info("Status Code", str(response.status_code))
            
            if response.status_code == 401:
                print_success("Request without token correctly rejected")
                return True
            else:
                print_error(f"Request without token not rejected: {response.text}")
                return False
                
        except Exception as e:
            print_error(f"No token test error: {e}")
            return False

async def test_server_connection():
    """Test if server is running"""
    print_header("Server Connection Test")
    
    async with httpx.AsyncClient() as client:
        try:
            response = await client.get(f"{BASE_URL}/health", timeout=5.0)
            
            if response.status_code == 200:
                print_success("Server is running")
                return True
            else:
                print_info("Health endpoint status", str(response.status_code))
                
        except httpx.ConnectError:
            print_error("Cannot connect to server")
            print_info("Expected URL", BASE_URL)
            print_info("Solution", "Start the backend server with: python -m uvicorn app.main:app --reload --host 127.0.0.1 --port 8000")
            return False
        except Exception as e:
            print_error(f"Connection test error: {e}")
            return False
    
    # Try alternative endpoint
    try:
        async with httpx.AsyncClient() as client:
            response = await client.get(f"{BASE_URL}/", timeout=5.0)
            if response.status_code in [200, 404]:
                print_success("Server is running (via root endpoint)")
                return True
    except:
        pass
    
    return False

async def main():
    """Main test function"""
    print_header("OHT-50 JWT Authentication Test")
    print_info("Purpose", "Test JWT token authentication system")
    print_info("Issue", "GitHub Issue #104 - JWT Token Authentication")
    print_info("Server", BASE_URL)
    
    # Test server connection first
    if not await test_server_connection():
        print_error("Server not available - skipping tests")
        return 1
    
    # Run tests
    test_results = []
    
    # Test login
    token = await test_login()
    test_results.append(("Login", token is not None))
    
    if token:
        # Test authenticated request
        auth_success = await test_authenticated_request(token)
        test_results.append(("Authenticated Request", auth_success))
        
        # Test token refresh
        refresh_success = await test_token_refresh(token)
        test_results.append(("Token Refresh", refresh_success))
    
    # Test invalid token
    invalid_success = await test_invalid_token()
    test_results.append(("Invalid Token Rejection", invalid_success))
    
    # Test no token
    no_token_success = await test_no_token()
    test_results.append(("No Token Rejection", no_token_success))
    
    # Summary
    print_header("Test Results Summary")
    passed = 0
    total = len(test_results)
    
    for test_name, result in test_results:
        if result:
            print_success(f"{test_name}: PASSED")
            passed += 1
        else:
            print_error(f"{test_name}: FAILED")
    
    print_info("Tests Passed", f"{passed}/{total}")
    
    if passed == total:
        print_success("All JWT authentication tests PASSED! ✨")
        print_info("Status", "Issue #104 should be resolved")
        return 0
    else:
        print_error(f"{total - passed} tests FAILED")
        print_info("Status", "Issue #104 needs further investigation")
        return 1

if __name__ == "__main__":
    exit_code = asyncio.run(main())
    sys.exit(exit_code)
