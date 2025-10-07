#!/usr/bin/env python3
"""
Simple WebSocket Test Client for OHT-50
Test WebSocket connection with proper handshake
"""

import socket
import base64
import hashlib
import time

def test_websocket_handshake():
    """Test WebSocket handshake manually"""
    try:
        # Create socket connection
        sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        sock.settimeout(10)
        
        print("🔌 Connecting to WebSocket server...")
        sock.connect(('localhost', 8081))
        print("✅ Connected to WebSocket server")
        
        # Generate WebSocket key
        key = base64.b64encode(b"test-key-12345").decode('utf-8')
        
        # Create WebSocket handshake request
        request = (
            "GET / HTTP/1.1\r\n"
            "Host: localhost:8081\r\n"
            "Upgrade: websocket\r\n"
            "Connection: Upgrade\r\n"
            f"Sec-WebSocket-Key: {key}\r\n"
            "Sec-WebSocket-Version: 13\r\n"
            "\r\n"
        )
        
        print(f"📤 Sending WebSocket handshake request...")
        sock.send(request.encode())
        
        # Read response
        print("📥 Reading WebSocket handshake response...")
        response = sock.recv(1024).decode('utf-8')
        print(f"Response: {response}")
        
        if "101 Switching Protocols" in response:
            print("✅ WebSocket handshake successful!")
            return True
        else:
            print("❌ WebSocket handshake failed")
            return False
            
    except Exception as e:
        print(f"❌ Error: {e}")
        return False
    finally:
        try:
            sock.close()
        except:
            pass

if __name__ == "__main__":
    print("🧪 Testing WebSocket handshake...")
    success = test_websocket_handshake()
    if success:
        print("🎉 WebSocket test PASSED!")
    else:
        print("💥 WebSocket test FAILED!")
