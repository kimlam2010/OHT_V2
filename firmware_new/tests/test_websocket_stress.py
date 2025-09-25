#!/usr/bin/env python3
"""
WebSocket Stress Test for OHT-50
Test WebSocket server with high load and rapid connections
"""

import asyncio
import websockets
import json
import time
import random

async def stress_client(client_id, message_count=10):
    """Stress test WebSocket client"""
    uri = "ws://localhost:8081"
    
    try:
        print(f"🔌 Stress Client {client_id}: Connecting...")
        
        async with websockets.connect(uri) as websocket:
            print(f"✅ Stress Client {client_id}: Connected!")
            
            # Send multiple messages rapidly
            for i in range(message_count):
                message = {
                    "type": "stress_test",
                    "client_id": client_id,
                    "message_id": i+1,
                    "timestamp": time.time(),
                    "data": f"Stress test message {i+1} from client {client_id}"
                }
                
                await websocket.send(json.dumps(message))
                print(f"📤 Client {client_id}: Sent message {i+1}/{message_count}")
                
                # Random delay between messages
                await asyncio.sleep(random.uniform(0.1, 0.5))
            
            # Wait for any responses
            try:
                response = await asyncio.wait_for(websocket.recv(), timeout=1.0)
                print(f"📥 Client {client_id}: Received: {response}")
            except asyncio.TimeoutError:
                print(f"⏰ Client {client_id}: No response")
            
            print(f"✅ Stress Client {client_id}: Completed!")
            
    except Exception as e:
        print(f"❌ Stress Client {client_id}: Error: {e}")

async def stress_test():
    """Run stress test with multiple clients"""
    print("🧪 WebSocket Stress Test...")
    
    # Create 5 concurrent stress clients
    tasks = []
    for i in range(5):
        task = asyncio.create_task(stress_client(i+1, message_count=5))
        tasks.append(task)
    
    # Wait for all clients to complete
    start_time = time.time()
    await asyncio.gather(*tasks)
    end_time = time.time()
    
    print(f"✅ Stress test completed in {end_time - start_time:.2f} seconds!")

if __name__ == "__main__":
    asyncio.run(stress_test())
