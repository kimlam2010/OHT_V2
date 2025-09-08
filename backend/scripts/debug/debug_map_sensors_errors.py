#!/usr/bin/env python3
"""
Debug script for Map, Sensors, and Localization 500 errors
"""

import requests
import json

# Configuration
base_url = "http://localhost:8000"

def login_as_admin():
    """Login as admin user"""
    login_data = {
        "username": "admin",
        "password": "admin123"
    }
    
    try:
        response = requests.post(f'{base_url}/api/v1/auth/login', json=login_data)
        if response.status_code == 200:
            data = response.json()
            return data.get('access_token')
        else:
            print(f"Login failed: {response.status_code}")
            return None
    except Exception as e:
        print(f"Login error: {e}")
        return None

def test_endpoint_with_error_details(endpoint, token):
    """Test endpoint and show detailed error"""
    headers = {"Authorization": f"Bearer {token}"}
    
    try:
        response = requests.get(f'{base_url}{endpoint}', headers=headers)
        print(f"\n{endpoint}: {response.status_code}")
        
        if response.status_code != 200:
            try:
                error_data = response.json()
                print(f"Error details: {json.dumps(error_data, indent=2)}")
            except:
                print(f"Error text: {response.text}")
        
        return response.status_code == 200
        
    except Exception as e:
        print(f"Request error: {e}")
        return False

def main():
    print("🔧 DEBUGGING MAP, SENSORS, LOCALIZATION 500 ERRORS")
    print("=" * 50)
    
    # Login
    token = login_as_admin()
    if not token:
        print("❌ Failed to login")
        return
    
    print("✅ Login successful")
    
    # Test problematic endpoints
    endpoints = [
        "/api/v1/map/current",
        "/api/v1/map/list", 
        "/api/v1/sensors/status",
        "/api/v1/sensors/list",
        "/api/v1/localization/position"
    ]
    
    for endpoint in endpoints:
        test_endpoint_with_error_details(endpoint, token)

if __name__ == "__main__":
    main()
