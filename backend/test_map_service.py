#!/usr/bin/env python3
"""
Test MapService directly
"""

import asyncio
import sys
import os

# Add the backend directory to Python path
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))

from app.services.map_service import MapService

async def test_map_service():
    """Test MapService methods"""
    print("🧪 Testing MapService...")
    
    try:
        # Initialize service
        map_service = MapService()
        print("✅ MapService initialized")
        
        # Test get_mapping_status
        print("\n🔍 Testing get_mapping_status...")
        status = map_service.get_mapping_status()
        print(f"✅ Status: {status}")
        
        # Test get_map_list
        print("\n🔍 Testing get_map_list...")
        try:
            map_list = await map_service.get_map_list()
            print(f"✅ Map list: {map_list}")
        except Exception as e:
            print(f"❌ Map list error: {e}")
        
        # Test get_robot_position
        print("\n🔍 Testing get_robot_position...")
        try:
            position = await map_service.get_robot_position()
            print(f"✅ Position: {position}")
        except Exception as e:
            print(f"❌ Position error: {e}")
            
    except Exception as e:
        print(f"❌ Test failed: {e}")
        import traceback
        traceback.print_exc()

if __name__ == "__main__":
    asyncio.run(test_map_service())
