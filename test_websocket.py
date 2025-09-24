#!/usr/bin/env python3
"""
WebSocket Test Script for OHT-50
Test WebSocket connection và functionality
"""

import asyncio
import websockets
import json
import time

async def test_websocket():
    """Test WebSocket connection"""
    uri = "ws://localhost:8080/ws"
    
    try:
        print("🔌 Connecting to WebSocket server...")
        async with websockets.connect(uri) as websocket:
            print("✅ WebSocket connected successfully!")
            
            # Test 1: Send ping message
            ping_msg = {"type": "ping", "timestamp": time.time()}
            await websocket.send(json.dumps(ping_msg))
            print(f"📤 Sent: {ping_msg}")
            
            # Test 2: Wait for response
            try:
                response = await asyncio.wait_for(websocket.recv(), timeout=5.0)
                print(f"📥 Received: {response}")
            except asyncio.TimeoutError:
                print("⏰ No response received (timeout)")
            
            # Test 3: Send status request
            status_msg = {"type": "status", "request": "system_status"}
            await websocket.send(json.dumps(status_msg))
            print(f"📤 Sent: {status_msg}")
            
            # Test 4: Wait for status response
            try:
                response = await asyncio.wait_for(websocket.recv(), timeout=5.0)
                print(f"📥 Received: {response}")
            except asyncio.TimeoutError:
                print("⏰ No status response received (timeout)")
            
            print("✅ WebSocket test completed successfully!")
            
    except websockets.exceptions.ConnectionRefused:
        print("❌ WebSocket connection refused - server not running")
    except Exception as e:
        print(f"❌ WebSocket test failed: {e}")

if __name__ == "__main__":
    print("🧪 OHT-50 WebSocket Test")
    print("=" * 40)
    asyncio.run(test_websocket())
