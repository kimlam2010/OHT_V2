#!/usr/bin/env python3
"""
Test script cho OHT-50 Firmware API
Test tất cả endpoints: HTTP REST API và WebSocket
"""

import requests
import json
import time
import websocket
import threading
from datetime import datetime

class FirmwareAPITester:
    def __init__(self, host="127.0.0.1", http_port=8080, ws_port=8081):
        self.host = host
        self.http_port = http_port
        self.ws_port = ws_port
        self.base_url = f"http://{host}:{http_port}"
        self.ws_url = f"ws://{host}:{ws_port}/ws"
        self.session = requests.Session()
        
    def test_http_endpoints(self):
        """Test tất cả HTTP REST API endpoints"""
        print("=" * 60)
        print("TESTING HTTP REST API ENDPOINTS")
        print("=" * 60)
        
        # Test 1: GET /api/v1/status
        print("\n1. Testing GET /api/v1/status")
        try:
            response = self.session.get(f"{self.base_url}/api/v1/status", timeout=5)
            print(f"   Status Code: {response.status_code}")
            print(f"   Response: {response.text}")
        except Exception as e:
            print(f"   ERROR: {e}")
        
        # Test 2: GET /api/v1/modules
        print("\n2. Testing GET /api/v1/modules")
        try:
            response = self.session.get(f"{self.base_url}/api/v1/modules", timeout=5)
            print(f"   Status Code: {response.status_code}")
            print(f"   Response: {response.text}")
        except Exception as e:
            print(f"   ERROR: {e}")
        
        # Test 3: GET /api/v1/telemetry
        print("\n3. Testing GET /api/v1/telemetry")
        try:
            response = self.session.get(f"{self.base_url}/api/v1/telemetry", timeout=5)
            print(f"   Status Code: {response.status_code}")
            print(f"   Response: {response.text}")
        except Exception as e:
            print(f"   ERROR: {e}")
        
        # Test 4: POST /api/v1/command
        print("\n4. Testing POST /api/v1/command")
        test_commands = [
            {"command": "status", "params": {}},
            {"command": "led_power", "params": {"state": "on"}},
            {"command": "led_system", "params": {"state": "blink_slow"}},
            {"command": "estop_status", "params": {}},
            {"command": "scan_modules", "params": {}}
        ]
        
        for i, cmd in enumerate(test_commands):
            print(f"   Command {i+1}: {cmd['command']}")
            try:
                response = self.session.post(
                    f"{self.base_url}/api/v1/command",
                    json=cmd,
                    timeout=5
                )
                print(f"   Status Code: {response.status_code}")
                print(f"   Response: {response.text}")
            except Exception as e:
                print(f"   ERROR: {e}")
        
        # Test 5: GET /api/v1/config
        print("\n5. Testing GET /api/v1/config")
        try:
            response = self.session.get(f"{self.base_url}/api/v1/config", timeout=5)
            print(f"   Status Code: {response.status_code}")
            print(f"   Response: {response.text}")
        except Exception as e:
            print(f"   ERROR: {e}")
        
        # Test 6: POST /api/v1/config
        print("\n6. Testing POST /api/v1/config")
        test_config = {
            "system": {
                "update_period_ms": 50,
                "safety_check_interval_ms": 50
            },
            "communication": {
                "rs485_baud_rate": 115200,
                "modbus_timeout_ms": 1000
            }
        }
        try:
            response = self.session.post(
                f"{self.base_url}/api/v1/config",
                json=test_config,
                timeout=5
            )
            print(f"   Status Code: {response.status_code}")
            print(f"   Response: {response.text}")
        except Exception as e:
            print(f"   ERROR: {e}")
        
        # Test 7: GET /api/v1/diagnostics
        print("\n7. Testing GET /api/v1/diagnostics")
        try:
            response = self.session.get(f"{self.base_url}/api/v1/diagnostics", timeout=5)
            print(f"   Status Code: {response.status_code}")
            print(f"   Response: {response.text}")
        except Exception as e:
            print(f"   ERROR: {e}")
        
        # Test 8: GET /api/v1/health
        print("\n8. Testing GET /api/v1/health")
        try:
            response = self.session.get(f"{self.base_url}/api/v1/health", timeout=5)
            print(f"   Status Code: {response.status_code}")
            print(f"   Response: {response.text}")
        except Exception as e:
            print(f"   ERROR: {e}")
    
    def test_websocket(self):
        """Test WebSocket connection và streaming"""
        print("\n" + "=" * 60)
        print("TESTING WEBSOCKET CONNECTION")
        print("=" * 60)
        
        try:
            # Test WebSocket connection
            print(f"Connecting to WebSocket: {self.ws_url}")
            ws = websocket.create_connection(self.ws_url, timeout=5)
            print("WebSocket connected successfully!")
            
            # Test sending message
            test_message = {
                "type": "subscribe",
                "data": {
                    "telemetry": True,
                    "events": True
                }
            }
            print(f"Sending message: {json.dumps(test_message)}")
            ws.send(json.dumps(test_message))
            
            # Wait for response
            print("Waiting for response...")
            response = ws.recv()
            print(f"Received: {response}")
            
            # Wait for more messages (telemetry)
            print("Waiting for telemetry data...")
            for i in range(3):
                try:
                    response = ws.recv()
                    print(f"Telemetry {i+1}: {response}")
                except websocket.WebSocketTimeoutException:
                    print(f"No telemetry data received in timeout {i+1}")
                    break
            
            ws.close()
            print("WebSocket connection closed")
            
        except Exception as e:
            print(f"WebSocket ERROR: {e}")
    
    def test_curl_commands(self):
        """Test bằng curl commands"""
        print("\n" + "=" * 60)
        print("TESTING WITH CURL COMMANDS")
        print("=" * 60)
        
        curl_tests = [
            f"curl -X GET {self.base_url}/api/v1/status",
            f"curl -X GET {self.base_url}/api/v1/modules",
            f"curl -X GET {self.base_url}/api/v1/telemetry",
            f"curl -X POST {self.base_url}/api/v1/command -H 'Content-Type: application/json' -d '{{\"command\": \"status\", \"params\": {{}}}}'",
            f"curl -X GET {self.base_url}/api/v1/config",
            f"curl -X GET {self.base_url}/api/v1/diagnostics",
            f"curl -X GET {self.base_url}/api/v1/health"
        ]
        
        for i, curl_cmd in enumerate(curl_tests):
            print(f"\n{i+1}. {curl_cmd}")
            try:
                import subprocess
                result = subprocess.run(curl_cmd, shell=True, capture_output=True, text=True, timeout=10)
                print(f"   Exit Code: {result.returncode}")
                print(f"   Output: {result.stdout}")
                if result.stderr:
                    print(f"   Error: {result.stderr}")
            except Exception as e:
                print(f"   ERROR: {e}")
    
    def run_all_tests(self):
        """Chạy tất cả tests"""
        print(f"OHT-50 Firmware API Test - {datetime.now()}")
        print(f"Target: {self.base_url}")
        
        # Test HTTP endpoints
        self.test_http_endpoints()
        
        # Test WebSocket
        self.test_websocket()
        
        # Test curl commands
        self.test_curl_commands()
        
        print("\n" + "=" * 60)
        print("TEST COMPLETED")
        print("=" * 60)

if __name__ == "__main__":
    # Test với firmware
    tester = FirmwareAPITester()
    tester.run_all_tests()
