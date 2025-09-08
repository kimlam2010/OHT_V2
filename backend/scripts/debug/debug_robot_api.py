#!/usr/bin/env python3
"""
Debug script for robot API endpoints
"""

import asyncio
import httpx
from app.main import app

async def test_robot_control():
    """Test robot control endpoint"""
    async with httpx.AsyncClient(app=app, base_url='http://test') as client:
        try:
            response = await client.post('/api/v1/robot/control', 
                json={'type': 'move', 'parameters': {'direction': 'forward'}})
            print(f'Status: {response.status_code}')
            print(f'Response: {response.text}')
        except Exception as e:
            print(f'Error: {e}')

async def test_emergency_stop():
    """Test emergency stop endpoint"""
    async with httpx.AsyncClient(app=app, base_url='http://test') as client:
        try:
            response = await client.post('/api/v1/robot/emergency-stop')
            print(f'Status: {response.status_code}')
            print(f'Response: {response.text}')
        except Exception as e:
            print(f'Error: {e}')

if __name__ == "__main__":
    print("Testing Robot Control API...")
    asyncio.run(test_robot_control())
    print("\nTesting Emergency Stop API...")
    asyncio.run(test_emergency_stop())
