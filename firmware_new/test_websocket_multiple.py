#!/usr/bin/env python3
"""
Multiple WebSocket Clients Test for OHT-50
Test WebSocket server with multiple concurrent connections
"""

import asyncio
import websockets
import json
import time

async def websocket_client(client_id):
    """Single WebSocket client"""
    uri = "ws://localhost:8081"
    
    try:
        print(f"🔌 Client {client_id}: Connecting to WebSocket server...")
        
        async with websockets.connect(uri) as websocket:
            print(f"✅ Client {client_id}: WebSocket connection established!")
            
            # Send client identification
            client_message = {
                "type": "client_hello",
                "client_id": client_id,
                "timestamp": time.time()
            }
            await websocket.send(json.dumps(client_message))
            print(f"📤 Client {client_id}: Sent hello message")
            
            # Wait for response
            try:
                response = await asyncio.wait_for(websocket.recv(), timeout=3.0)
                print(f"📥 Client {client_id}: Received response: {response}")
            except asyncio.TimeoutError:
                print(f"⏰ Client {client_id}: No response within 3 seconds")
            
            # Send ping
            await websocket.ping()
            print(f"🏓 Client {client_id}: Ping sent")
            
            # Wait a bit
            await asyncio.sleep(1)
            
            print(f"✅ Client {client_id}: Test completed!")
            
    except Exception as e:
        print(f"❌ Client {client_id}: Error: {e}")

async def test_multiple_clients():
    """Test multiple WebSocket clients"""
    print("🧪 Testing Multiple WebSocket Clients...")
    
    # Create 3 concurrent clients
    tasks = []
    for i in range(3):
        task = asyncio.create_task(websocket_client(i+1))
        tasks.append(task)
    
    # Wait for all clients to complete
    await asyncio.gather(*tasks)
    
    print("✅ Multiple WebSocket clients test completed!")

if __name__ == "__main__":
    asyncio.run(test_multiple_clients())
