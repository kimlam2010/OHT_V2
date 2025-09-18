#!/usr/bin/env python3
"""
Test script for RS485 API
Demonstrates how to use the new RS485 endpoints
"""

import asyncio
import os
import sys
import json
from pathlib import Path

# Add backend to path
backend_path = Path(__file__).parent
sys.path.insert(0, str(backend_path))

# Set environment for testing
os.environ["TESTING"] = "true"
os.environ["USE_MOCK_FIRMWARE"] = "true"  # This enables RS485 mock
os.environ["JWT_SECRET"] = "test-secret-key-for-rs485-demo"

from app.services.rs485_service import rs485_service


async def test_rs485_api():
    """Test RS485 API functionality"""
    print("🚀 Testing RS485 API with Mock Data")
    print("=" * 50)
    
    # Initialize service
    print("\n📋 1. Initializing RS485 Service...")
    initialized = await rs485_service.initialize()
    print(f"✅ RS485 Service initialized: {initialized}")
    
    # Get all modules
    print("\n📋 2. Getting all RS485 modules...")
    modules = await rs485_service.get_modules()
    print(f"✅ Found {len(modules)} RS485 modules:")
    for module in modules:
        status_icon = {"healthy": "✅", "warning": "⚠️", "error": "❌", "lost": "💀"}.get(module.status, "❓")
        mandatory_icon = "🔴" if module.mandatory else "🔵"
        print(f"   {status_icon} {mandatory_icon} {module.addr} - {module.name} ({module.type}) - {module.status}")
        print(f"      Battery: {module.real_time.battery:.1f}%, Temp: {module.real_time.temperature:.1f}°C")
    
    # Get specific module
    print("\n📋 3. Getting specific module (0x02 - Power)...")
    power_module = await rs485_service.get_module(2)
    if power_module:
        print(f"✅ Power Module Details:")
        print(f"   Address: {power_module.addr}")
        print(f"   Name: {power_module.name}")
        print(f"   Status: {power_module.status}")
        print(f"   FW Version: {power_module.fw_version}")
        print(f"   Last Seen: {power_module.last_seen}")
        print(f"   Error Rate: {power_module.error_rate:.2f}%")
        print(f"   Response Time: {power_module.response_time}ms")
        print(f"   Real-time Data:")
        print(f"     Battery: {power_module.real_time.battery:.1f}%")
        print(f"     Voltage: {power_module.real_time.voltage:.1f}V")
        print(f"     Current: {power_module.real_time.current:.1f}A")
        print(f"     Temperature: {power_module.real_time.temperature:.1f}°C")
    
    # Get bus health
    print("\n📋 4. Getting RS485 bus health...")
    bus_health = await rs485_service.get_bus_health()
    print(f"✅ RS485 Bus Health:")
    print(f"   Status: {bus_health.status}")
    print(f"   Error Rate: {bus_health.error_rate:.2f}%")
    print(f"   Response Time P95: {bus_health.response_time_p95}ms")
    print(f"   Throughput: {bus_health.throughput} fps")
    print(f"   Total Modules: {bus_health.total_modules}")
    print(f"   Active Modules: {bus_health.active_modules}")
    print(f"   Failed Modules: {bus_health.failed_modules}")
    
    # Start discovery
    print("\n📋 5. Starting RS485 module discovery...")
    discovery_status = await rs485_service.start_discovery()
    print(f"✅ Discovery started: {discovery_status.status_message}")
    
    # Wait for discovery to complete
    print("   Waiting for discovery to complete...")
    await asyncio.sleep(3)  # Discovery takes ~2 seconds
    
    # Get discovery status
    final_status = await rs485_service.get_discovery_status()
    print(f"✅ Discovery completed: {final_status.status_message}")
    print(f"   Progress: {final_status.progress}%")
    print(f"   Modules Found: {final_status.modules_found}")
    
    # Get discovery results
    print("\n📋 6. Getting discovery results...")
    results = await rs485_service.get_discovery_results()
    print(f"✅ Discovery Results ({len(results)} modules):")
    for result in results:
        found_icon = "✅" if result.found else "❌"
        conflict_icon = "⚠️" if result.conflict else ""
        print(f"   {found_icon} {result.addr} - {result.name} ({result.type}) {conflict_icon}")
        if result.response_time:
            print(f"      Response Time: {result.response_time}ms")
    
    # Test module actions
    print("\n📋 7. Testing module actions...")
    
    # Ping module
    print("   Pinging Power module (0x02)...")
    ping_result = await rs485_service.ping_module(2)
    if ping_result.get("success"):
        print(f"   ✅ Ping successful: {ping_result.get('message')}")
        print(f"      Response Time: {ping_result.get('response_time')}ms")
    else:
        print(f"   ❌ Ping failed: {ping_result.get('message')}")
    
    # Reset module
    print("   Resetting Safety module (0x03)...")
    reset_result = await rs485_service.reset_module(3)
    if reset_result.get("success"):
        print(f"   ✅ Reset successful: {reset_result.get('message')}")
    else:
        print(f"   ❌ Reset failed: {reset_result.get('message')}")
    
    # Restart bus
    print("\n📋 8. Restarting RS485 bus...")
    restart_result = await rs485_service.restart_bus()
    if restart_result.get("success"):
        print(f"✅ Bus restart successful: {restart_result.get('message')}")
    else:
        print(f"❌ Bus restart failed: {restart_result.get('message')}")
    
    print("\n🎉 RS485 API Test Completed!")
    print("=" * 50)
    print("✅ All RS485 mock functionality working correctly!")
    print("🔧 To use with real firmware, set USE_MOCK_FIRMWARE=false")


if __name__ == "__main__":
    asyncio.run(test_rs485_api())

