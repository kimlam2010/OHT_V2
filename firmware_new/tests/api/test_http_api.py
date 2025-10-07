#!/usr/bin/env python3
"""
Simple HTTP API Test Client for OHT-50
Test HTTP API endpoints
"""

import socket
import time

def test_http_endpoint(path):
    """Test HTTP endpoint"""
    try:
        # Create socket connection
        sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        sock.settimeout(5)
        
        print(f"🔌 Connecting to HTTP server for {path}...")
        sock.connect(('localhost', 8081))
        print("✅ Connected to HTTP server")
        
        # Create HTTP request
        request = (
            f"GET {path} HTTP/1.1\r\n"
            "Host: localhost:8081\r\n"
            "Connection: close\r\n"
            "\r\n"
        )
        
        print(f"📤 Sending HTTP request for {path}...")
        sock.send(request.encode())
        
        # Read response
        print("📥 Reading HTTP response...")
        response = sock.recv(4096).decode('utf-8')
        print(f"Response: {response}")
        
        if "200 OK" in response:
            print(f"✅ HTTP {path} test successful!")
            return True
        else:
            print(f"❌ HTTP {path} test failed")
            return False
            
    except Exception as e:
        print(f"❌ Error testing {path}: {e}")
        return False
    finally:
        try:
            sock.close()
        except:
            pass

def test_all_endpoints():
    """Test all HTTP endpoints"""
    endpoints = [
        "/health",
        "/api/v1/status", 
        "/api/v1/robot/status"
    ]
    
    results = []
    for endpoint in endpoints:
        print(f"\n🧪 Testing {endpoint}...")
        success = test_http_endpoint(endpoint)
        results.append((endpoint, success))
        time.sleep(1)
    
    return results

if __name__ == "__main__":
    print("🧪 Testing HTTP API endpoints...")
    results = test_all_endpoints()
    
    print("\n📊 Test Results:")
    for endpoint, success in results:
        status = "✅ PASS" if success else "❌ FAIL"
        print(f"  {endpoint}: {status}")
    
    passed = sum(1 for _, success in results if success)
    total = len(results)
    print(f"\n🎯 Summary: {passed}/{total} tests passed")
