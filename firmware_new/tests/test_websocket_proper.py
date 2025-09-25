#!/usr/bin/env python3
"""
Proper WebSocket Test Client for OHT-50
Test WebSocket connection with proper WebSocket protocol
"""

import asyncio
import websockets
import json
import time

async def test_websocket_connection():
    """Test WebSocket connection to OHT-50"""
    uri = "ws://localhost:8081"
    
    try:
        print(f"🔌 Connecting to WebSocket server at {uri}...")
        
        async with websockets.connect(uri) as websocket:
            print("✅ WebSocket connection established!")
            
            # Send test message
            test_message = {
                "type": "test",
                "message": "Hello from Python WebSocket client",
                "timestamp": time.time()
            }
            
            await websocket.send(json.dumps(test_message))
            print(f"📤 Sent test message: {test_message}")
            
            # Wait for response
            try:
                response = await asyncio.wait_for(websocket.recv(), timeout=5.0)
                print(f"📥 Received response: {response}")
                
                # Try to parse as JSON
                try:
                    response_data = json.loads(response)
                    print(f"📊 Parsed JSON response: {response_data}")
                except json.JSONDecodeError:
                    print(f"📄 Raw response: {response}")
                    
            except asyncio.TimeoutError:
                print("⏰ No response received within 5 seconds")
            
            # Send ping
            await websocket.ping()
            print("🏓 Ping sent")
            
            # Wait a bit more
            await asyncio.sleep(2)
            
            print("✅ WebSocket test completed successfully!")
            
    except websockets.exceptions.ConnectionRefused:
        print("❌ Connection refused - WebSocket server not running")
    except websockets.exceptions.InvalidHandshake:
        print("❌ Invalid handshake - WebSocket server not responding properly")
    except Exception as e:
        print(f"❌ Error: {e}")

if __name__ == "__main__":
    print("🧪 Testing WebSocket connection to OHT-50...")
    asyncio.run(test_websocket_connection())
