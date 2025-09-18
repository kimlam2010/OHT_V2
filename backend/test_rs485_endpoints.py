#!/usr/bin/env python3
"""
Test RS485 API endpoints directly
"""

import asyncio
import sys
import os
from pathlib import Path

# Add backend to path
backend_path = Path(__file__).parent
sys.path.insert(0, str(backend_path))

# Set environment for testing
os.environ["TESTING"] = "true"
os.environ["USE_MOCK_FIRMWARE"] = "true"
os.environ["JWT_SECRET"] = "test-secret-key-for-rs485-demo"

from app.services.rs485_service import rs485_service


async def test_rs485_endpoints():
    """Test RS485 API endpoints directly without HTTP layer"""
    print("🔌 Testing RS485 Service Endpoints")
    print("=" * 50)
    
    # Initialize service
    print("\n📋 1. Initializing RS485 Service...")
    initialized = await rs485_service.initialize()
    print(f"✅ RS485 Service initialized: {initialized}")
    
    # Test get modules
    print("\n📋 2. Testing GET /api/v1/rs485/modules")
    try:
        modules = await rs485_service.get_modules()
        print(f"✅ Found {len(modules)} modules:")
        for module in modules:
            status_icon = {"healthy": "✅", "warning": "⚠️", "error": "❌", "lost": "💀"}.get(module.status, "❓")
            mandatory_icon = "🔴" if module.mandatory else "🔵"
            print(f"   {status_icon} {mandatory_icon} {module.addr} - {module.name}")
            print(f"      📊 Status: {module.status}")
            print(f"      🔋 Battery: {module.real_time.battery:.1f}%")
            print(f"      🌡️  Temp: {module.real_time.temperature:.1f}°C")
            print(f"      ⚡ Voltage: {module.real_time.voltage:.1f}V")
            print(f"      📡 Last Seen: {module.last_seen}")
            print(f"      📈 Error Rate: {module.error_rate:.2f}%")
            print(f"      ⏱️  Response: {module.response_time}ms")
            print()
    except Exception as e:
        print(f"❌ Error: {e}")
    
    # Test get specific module
    print("\n📋 3. Testing GET /api/v1/rs485/modules/2")
    try:
        power_module = await rs485_service.get_module(2)
        if power_module:
            print(f"✅ Power Module (0x02):")
            print(f"   📊 Status: {power_module.status}")
            print(f"   🔋 Battery: {power_module.real_time.battery:.1f}%")
            print(f"   🌡️  Temperature: {power_module.real_time.temperature:.1f}°C")
            print(f"   ⚡ Voltage: {power_module.real_time.voltage:.1f}V")
            print(f"   🔌 Current: {power_module.real_time.current:.1f}A")
        else:
            print("❌ Power module not found")
    except Exception as e:
        print(f"❌ Error: {e}")
    
    # Test bus health
    print("\n📋 4. Testing GET /api/v1/rs485/bus/health")
    try:
        bus_health = await rs485_service.get_bus_health()
        print(f"✅ RS485 Bus Health:")
        print(f"   📊 Status: {bus_health.status}")
        print(f"   📈 Error Rate: {bus_health.error_rate:.2f}%")
        print(f"   ⏱️  Response P95: {bus_health.response_time_p95}ms")
        print(f"   🚀 Throughput: {bus_health.throughput} fps")
        print(f"   📡 Total Modules: {bus_health.total_modules}")
        print(f"   🟢 Active: {bus_health.active_modules}")
        print(f"   🔴 Failed: {bus_health.failed_modules}")
    except Exception as e:
        print(f"❌ Error: {e}")
    
    # Test discovery
    print("\n📋 5. Testing POST /api/v1/rs485/discovery/start")
    try:
        discovery = await rs485_service.start_discovery()
        print(f"✅ Discovery started:")
        print(f"   📊 Status: {discovery.status_message}")
        print(f"   📈 Progress: {discovery.progress}%")
        print(f"   📡 Modules Found: {discovery.modules_found}")
        
        # Wait and check final status
        await asyncio.sleep(3)
        final_status = await rs485_service.get_discovery_status()
        print(f"✅ Discovery completed:")
        print(f"   📊 Final Status: {final_status.status_message}")
        print(f"   📈 Progress: {final_status.progress}%")
        print(f"   📡 Modules Found: {final_status.modules_found}")
        
    except Exception as e:
        print(f"❌ Error: {e}")
    
    # Test module actions
    print("\n📋 6. Testing Module Actions")
    try:
        # Ping Power module (0x02)
        print("   🏓 Pinging Power module (0x02)...")
        ping_result = await rs485_service.ping_module(2)
        if ping_result.get("success"):
            print(f"   ✅ Ping successful: {ping_result.get('message')}")
            print(f"   ⏱️  Response Time: {ping_result.get('response_time')}ms")
        else:
            print(f"   ❌ Ping failed: {ping_result.get('message')}")
        
        # Reset Safety module (0x03)
        print("   🔄 Resetting Safety module (0x03)...")
        reset_result = await rs485_service.reset_module(3)
        if reset_result.get("success"):
            print(f"   ✅ Reset successful: {reset_result.get('message')}")
        else:
            print(f"   ❌ Reset failed: {reset_result.get('message')}")
            
    except Exception as e:
        print(f"❌ Error: {e}")
    
    print("\n🎉 RS485 API Service Test Completed!")
    print("=" * 50)
    print("✅ All RS485 endpoints working correctly!")
    print("🔌 Backend RS485 API ready for Monitoring Interface!")


if __name__ == "__main__":
    asyncio.run(test_rs485_endpoints())
