#!/usr/bin/env python3
"""
Test script to verify Issue #136 fixes
"""

import requests
import json
import sys

BASE_URL = "http://localhost:8000"

def test_health():
    """Test /health endpoint"""
    try:
        response = requests.get(f"{BASE_URL}/health")
        print(f"✅ /health: {response.status_code}")
        if response.status_code == 200:
            data = response.json()
            print(f"   Status: {data.get('status')}")
        return response.status_code == 200
    except Exception as e:
        print(f"❌ /health failed: {e}")
        return False

def test_auth_me():
    """Test /api/v1/auth/me endpoint"""
    try:
        # Try without token first to see if 500 error is fixed
        response = requests.get(f"{BASE_URL}/api/v1/auth/me")
        print(f"✅ /api/v1/auth/me: {response.status_code}")
        if response.status_code != 500:
            print(f"   Fixed 500 error - now returns: {response.status_code}")
            return True
        else:
            print(f"   Still returns 500 error")
            return False
    except Exception as e:
        print(f"❌ /api/v1/auth/me failed: {e}")
        return False

def test_robot_control():
    """Test /api/v1/robot/control endpoint"""
    try:
        # Test with missing command_type
        response = requests.post(f"{BASE_URL}/api/v1/robot/control", 
                               json={"parameters": {"test": "value"}})
        print(f"✅ /api/v1/robot/control (missing command_type): {response.status_code}")
        if response.status_code == 422:
            print(f"   Correctly returns 422 for missing command_type")
            return True
        else:
            print(f"   Unexpected status: {response.status_code}")
            return False
    except Exception as e:
        print(f"❌ /api/v1/robot/control failed: {e}")
        return False

def test_robot_battery():
    """Test /api/v1/robot/battery endpoint"""
    try:
        response = requests.get(f"{BASE_URL}/api/v1/robot/battery")
        print(f"✅ /api/v1/robot/battery: {response.status_code}")
        if response.status_code == 401:  # Not authenticated is expected
            print(f"   Endpoint exists (401 = auth required)")
            return True
        elif response.status_code == 200:
            print(f"   Endpoint working!")
            return True
        else:
            print(f"   Unexpected status: {response.status_code}")
            return False
    except Exception as e:
        print(f"❌ /api/v1/robot/battery failed: {e}")
        return False

def test_emergency_stop():
    """Test /api/v1/robot/emergency-stop endpoint"""
    try:
        response = requests.post(f"{BASE_URL}/api/v1/robot/emergency-stop")
        print(f"✅ /api/v1/robot/emergency-stop: {response.status_code}")
        if response.status_code == 401:  # Not authenticated is expected
            print(f"   Endpoint exists (401 = auth required)")
            return True
        elif response.status_code == 200:
            print(f"   Endpoint working!")
            return True
        else:
            print(f"   Unexpected status: {response.status_code}")
            return False
    except Exception as e:
        print(f"❌ /api/v1/robot/emergency-stop failed: {e}")
        return False

def test_monitoring_endpoints():
    """Test monitoring endpoints"""
    endpoints = [
        "/api/v1/monitoring/performance",
        "/api/v1/monitoring/metrics", 
        "/api/v1/monitoring/current",
        "/api/v1/monitoring/alerts",
        "/api/v1/monitoring/logs"
    ]
    
    results = []
    for endpoint in endpoints:
        try:
            response = requests.get(f"{BASE_URL}{endpoint}")
            print(f"✅ {endpoint}: {response.status_code}")
            if response.status_code == 401:  # Not authenticated is expected
                print(f"   Endpoint exists (401 = auth required)")
                results.append(True)
            elif response.status_code == 200:
                print(f"   Endpoint working!")
                results.append(True)
            else:
                print(f"   Unexpected status: {response.status_code}")
                results.append(False)
        except Exception as e:
            print(f"❌ {endpoint} failed: {e}")
            results.append(False)
    
    return all(results)

def test_websocket_status():
    """Test WebSocket /ws/status endpoint"""
    try:
        # Test HTTP endpoint first
        response = requests.get(f"{BASE_URL}/ws/status")
        print(f"✅ /ws/status (HTTP): {response.status_code}")
        if response.status_code == 401:  # Not authenticated is expected
            print(f"   HTTP endpoint exists (401 = auth required)")
            return True
        else:
            print(f"   Unexpected status: {response.status_code}")
            return False
    except Exception as e:
        print(f"❌ /ws/status failed: {e}")
        return False

def main():
    """Run all tests"""
    print("🧪 Testing Issue #136 Fixes")
    print("=" * 50)
    
    tests = [
        ("Health Check", test_health),
        ("Auth /me Fix", test_auth_me),
        ("Robot Control Fix", test_robot_control),
        ("Robot Battery Endpoint", test_robot_battery),
        ("Emergency Stop Fix", test_emergency_stop),
        ("Monitoring Endpoints", test_monitoring_endpoints),
        ("WebSocket Status", test_websocket_status),
    ]
    
    results = []
    for test_name, test_func in tests:
        print(f"\n🔍 Testing: {test_name}")
        result = test_func()
        results.append(result)
    
    print("\n" + "=" * 50)
    print("📊 Test Results Summary:")
    
    passed = sum(results)
    total = len(results)
    
    for i, (test_name, _) in enumerate(tests):
        status = "✅ PASS" if results[i] else "❌ FAIL"
        print(f"  {status} {test_name}")
    
    print(f"\n🎯 Overall: {passed}/{total} tests passed")
    
    if passed == total:
        print("🎉 All fixes working correctly!")
        return 0
    else:
        print("⚠️  Some fixes need attention")
        return 1

if __name__ == "__main__":
    sys.exit(main())
