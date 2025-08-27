#!/usr/bin/env python3
"""
Simple HTTP Server để test API endpoints của OHT-50 Firmware
"""

import json
import time
from http.server import HTTPServer, BaseHTTPRequestHandler
from urllib.parse import urlparse

class OHT50APIHandler(BaseHTTPRequestHandler):
    def __init__(self, *args, **kwargs):
        self.api_endpoints = {
            '/api/v1/system/status': self.handle_system_status,
            '/api/v1/system/health': self.handle_system_health,
            '/api/v1/network/status': self.handle_network_status,
            '/api/v1/communication/status': self.handle_communication_status,
            '/api/v1/safety/status': self.handle_safety_status,
            '/api/v1/config': self.handle_config,
        }
        super().__init__(*args, **kwargs)
    
    def do_GET(self):
        try:
            parsed_url = urlparse(self.path)
            path = parsed_url.path
            
            if path in self.api_endpoints:
                response = self.api_endpoints[path]()
                self.send_response(200)
                self.send_header('Content-Type', 'application/json')
                self.send_header('Access-Control-Allow-Origin', '*')
                self.end_headers()
                self.wfile.write(json.dumps(response).encode())
            else:
                self.send_error(404, "Endpoint not found")
        except Exception as e:
            self.send_error(500, f"Internal server error: {str(e)}")
    
    def do_POST(self):
        try:
            parsed_url = urlparse(self.path)
            path = parsed_url.path
            
            if path == '/api/v1/config':
                content_length = int(self.headers['Content-Length'])
                post_data = self.rfile.read(content_length)
                response = self.handle_config_post(post_data)
                self.send_response(200)
                self.send_header('Content-Type', 'application/json')
                self.send_header('Access-Control-Allow-Origin', '*')
                self.end_headers()
                self.wfile.write(json.dumps(response).encode())
            else:
                self.send_error(404, "Endpoint not found")
        except Exception as e:
            self.send_error(500, f"Internal server error: {str(e)}")
    
    def do_OPTIONS(self):
        self.send_response(200)
        self.send_header('Access-Control-Allow-Origin', '*')
        self.send_header('Access-Control-Allow-Methods', 'GET, POST, PUT, DELETE, OPTIONS')
        self.send_header('Access-Control-Allow-Headers', 'Content-Type')
        self.end_headers()
    
    def handle_system_status(self):
        return {
            "system": {
                "state": "IDLE",
                "uptime": int(time.time() * 1000),
                "version": "1.0.0",
                "ready": True,
                "safety_ok": True,
                "communication_ok": True
            }
        }
    
    def handle_system_health(self):
        return {
            "status": "healthy",
            "timestamp": int(time.time() * 1000),
            "response_time_ms": 5,
            "details": "System is running normally"
        }
    
    def handle_network_status(self):
        return {
            "network": {
                "ethernet": {"connected": True, "ip": "192.168.1.100"},
                "wifi": {"connected": False, "ssid": ""},
                "rs485": {"connected": True, "baudrate": 115200, "devices_online": 1}
            }
        }
    
    def handle_communication_status(self):
        return {
            "communication": {
                "rs485": {"status": "active", "baudrate": 115200},
                "ethernet": {"status": "active", "port": 8080},
                "websocket": {"status": "active", "port": 8081, "clients": 0}
            }
        }
    
    def handle_safety_status(self):
        return {
            "safety": {
                "estop": {"triggered": False, "pin_status": "safe", "fault": False},
                "safety_level": 0,
                "fault_count": 0,
                "last_check": int(time.time() * 1000)
            }
        }
    
    def handle_config(self):
        return {
            "config": {
                "system": {"name": "OHT-50 Master Module", "version": "1.0.0"},
                "network": {"ethernet_port": 8080, "websocket_port": 8081, "rs485_baudrate": 115200},
                "safety": {"estop_pin": 59, "check_interval_ms": 100}
            }
        }
    
    def handle_config_post(self, post_data):
        try:
            config = json.loads(post_data.decode())
            return {
                "success": True,
                "message": "Configuration updated successfully",
                "timestamp": int(time.time() * 1000)
            }
        except json.JSONDecodeError:
            return {
                "success": False,
                "message": "Invalid JSON format",
                "timestamp": int(time.time() * 1000)
            }

def start_api_server(port=8080):
    server_address = ('', port)
    httpd = HTTPServer(server_address, OHT50APIHandler)
    print(f"Starting OHT-50 API Server on port {port}")
    print(f"Server running at http://localhost:{port}")
    print("Press Ctrl+C to stop")
    
    try:
        httpd.serve_forever()
    except KeyboardInterrupt:
        print("\nShutting down server...")
        httpd.shutdown()

if __name__ == "__main__":
    start_api_server(8080)
