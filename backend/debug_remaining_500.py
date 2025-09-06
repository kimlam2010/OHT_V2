#!/usr/bin/env python3
"""
Debug script for remaining 500 errors
"""

import requests
import json

def test_endpoint_with_details(url, headers, name):
    """Test endpoint and show detailed error information"""
    try:
        response = requests.get(url, headers=headers, timeout=10)
        print(f"\n🔍 {name}")
        print(f"   URL: {url}")
        print(f"   Status: {response.status_code}")
        
        if response.status_code == 200:
            print(f"   ✅ SUCCESS")
        else:
            print(f"   ❌ ERROR")
            try:
                error_data = response.json()
                print(f"   Error details: {json.dumps(error_data, indent=6)}")
            except:
                print(f"   Error text: {response.text}")
                
    except Exception as e:
        print(f"   💥 EXCEPTION: {e}")

def main():
    base_url = 'http://localhost:8000'
    
    # Login as admin
    print("🔐 Logging in as admin...")
    login_data = {'username': 'admin', 'password': 'admin123'}
    response = requests.post(f'{base_url}/api/v1/auth/login', json=login_data)
    
    if response.status_code != 200:
        print(f"❌ Login failed: {response.status_code}")
        return
        
    token = response.json().get('access_token')
    headers = {'Authorization': f'Bearer {token}'}
    print("✅ Login successful!")
    
    # Test problematic endpoints
    problematic_endpoints = [
        (f'{base_url}/api/v1/robot/status', 'Robot Status'),
        (f'{base_url}/api/v1/map/status', 'Map Status'),
        (f'{base_url}/api/v1/map/list', 'Map List'),
        (f'{base_url}/api/v1/localization/position', 'Localization Position'),
    ]
    
    print("\n🔍 TESTING PROBLEMATIC ENDPOINTS:")
    print("=" * 60)
    
    for url, name in problematic_endpoints:
        test_endpoint_with_details(url, headers, name)

if __name__ == "__main__":
    main()
