#!/usr/bin/env python3
"""
Test script đơn giản cho OHT-50 Firmware API
Test các endpoints cơ bản: system/status, system/health, modules, telemetry, command
"""

import requests
import json
import time
from datetime import datetime

class SimpleAPITester:
    def __init__(self, host="127.0.0.1", port=8080):
        self.host = host
        self.port = port
        self.base_url = f"http://{host}:{port}"
        self.session = requests.Session()
        
    def test_endpoint(self, endpoint, method="GET", data=None, expected_status=200):
        """Test một endpoint cụ thể"""
        url = f"{self.base_url}{endpoint}"
        print(f"\n{'='*60}")
        print(f"Testing {method} {endpoint}")
        print(f"{'='*60}")
        
        try:
            if method == "GET":
                response = self.session.get(url, timeout=5)
            elif method == "POST":
                response = self.session.post(url, json=data, timeout=5)
            elif method == "PUT":
                response = self.session.put(url, json=data, timeout=5)
            
            print(f"Status Code: {response.status_code}")
            print(f"Response Time: {response.elapsed.total_seconds():.3f}s")
            
            if response.status_code == expected_status:
                print("✅ PASS")
            else:
                print("❌ FAIL - Unexpected status code")
            
            try:
                json_response = response.json()
                print(f"JSON Response: {json.dumps(json_response, indent=2)}")
            except:
                print(f"Text Response: {response.text}")
                
        except requests.exceptions.ConnectionError:
            print("❌ FAIL - Connection refused (server not running)")
        except requests.exceptions.Timeout:
            print("❌ FAIL - Request timeout")
        except Exception as e:
            print(f"❌ FAIL - Error: {e}")
    
    def run_all_tests(self):
        """Chạy tất cả tests"""
        print("OHT-50 API Test - Simple Version")
        print(f"Target: {self.base_url}")
        print(f"Time: {datetime.now().strftime('%Y-%m-%d %H:%M:%S')}")
        
        # Test 1: System Status
        self.test_endpoint("/api/v1/system/status", "GET")
        
        # Test 2: System Health
        self.test_endpoint("/api/v1/system/health", "GET")
        
        # Test 3: Modules List
        self.test_endpoint("/api/v1/modules", "GET")
        
        # Test 4: Telemetry
        self.test_endpoint("/api/v1/telemetry", "GET")
        
        # Test 5: Command (POST)
        test_commands = [
            {"command": "status", "params": {}},
            {"command": "led_power", "params": {"state": "on"}},
            {"command": "estop_status", "params": {}}
        ]
        
        for i, cmd in enumerate(test_commands):
            self.test_endpoint("/api/v1/command", "POST", cmd)
        
        # Test 6: Configuration
        self.test_endpoint("/api/v1/config", "GET")
        
        # Test 7: Safety Status
        self.test_endpoint("/api/v1/safety/status", "GET")
        
        # Test 8: Diagnostics
        self.test_endpoint("/api/v1/diagnostics", "GET")
        
        print(f"\n{'='*60}")
        print("TEST COMPLETED")
        print(f"{'='*60}")

if __name__ == "__main__":
    tester = SimpleAPITester()
    tester.run_all_tests()
