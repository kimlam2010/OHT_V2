#!/usr/bin/env python3
"""
Advanced WebSocket Test Client for OHT-50
Test WebSocket connection with multiple messages and different types
"""

import asyncio
import websockets
import json
import time

async def test_websocket_advanced():
    """Test WebSocket connection with multiple message types"""
    uri = "ws://localhost:8081"
    
    try:
        print(f"🔌 Connecting to WebSocket server at {uri}...")
        
        async with websockets.connect(uri) as websocket:
            print("✅ WebSocket connection established!")
            
            # Test 1: Basic message
            print("\n📤 Test 1: Basic message")
            basic_message = {
                "type": "test",
                "message": "Basic test message",
                "timestamp": time.time()
            }
            await websocket.send(json.dumps(basic_message))
            print(f"📤 Sent: {basic_message}")
            
            # Test 2: Robot command
            print("\n📤 Test 2: Robot command")
            robot_command = {
                "type": "robot_command",
                "command": "move",
                "direction": "forward",
                "speed": 50,
                "timestamp": time.time()
            }
            await websocket.send(json.dumps(robot_command))
            print(f"📤 Sent: {robot_command}")
            
            # Test 3: Status request
            print("\n📤 Test 3: Status request")
            status_request = {
                "type": "status_request",
                "request": "robot_status",
                "timestamp": time.time()
            }
            await websocket.send(json.dumps(status_request))
            print(f"📤 Sent: {status_request}")
            
            # Test 4: Ping/Pong
            print("\n🏓 Test 4: Ping/Pong")
            await websocket.ping()
            print("🏓 Ping sent")
            
            # Wait for responses
            print("\n📥 Waiting for responses...")
            for i in range(5):
                try:
                    response = await asyncio.wait_for(websocket.recv(), timeout=2.0)
                    print(f"📥 Response {i+1}: {response}")
                    
                    # Try to parse as JSON
                    try:
                        response_data = json.loads(response)
                        print(f"📊 Parsed JSON: {response_data}")
                    except json.JSONDecodeError:
                        print(f"📄 Raw response: {response}")
                        
                except asyncio.TimeoutError:
                    print(f"⏰ No response {i+1} within 2 seconds")
                    break
            
            print("\n✅ Advanced WebSocket test completed!")
            
    except websockets.exceptions.ConnectionRefused:
        print("❌ Connection refused - WebSocket server not running")
    except websockets.exceptions.InvalidHandshake:
        print("❌ Invalid handshake - WebSocket server not responding properly")
    except Exception as e:
        print(f"❌ Error: {e}")

if __name__ == "__main__":
    print("🧪 Testing Advanced WebSocket connection to OHT-50...")
    asyncio.run(test_websocket_advanced())
