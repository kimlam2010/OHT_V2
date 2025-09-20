#!/usr/bin/env python3
"""
Test script để kiểm tra firmware integration fix
"""

import os
import sys
import asyncio

# Set environment variables
os.environ['USE_MOCK_FIRMWARE'] = 'false'
os.environ['TESTING'] = 'false'
os.environ['ENVIRONMENT'] = 'development'

# Add backend to path
sys.path.insert(0, '.')

from app.services.firmware_integration_service import get_firmware_service, MockFirmwareService
from app.services.telemetry import telemetry_service
from app.services.safety import safety_service


async def test_firmware_services():
    """Test các firmware services"""
    print("🔍 Testing Firmware Integration Fix...")
    print("=" * 50)
    
    # Test 1: get_firmware_service function
    print("\n1️⃣ Testing get_firmware_service():")
    service = get_firmware_service(use_mock=False)
    print(f"   Service type: {type(service).__name__}")
    if hasattr(service, 'firmware_url'):
        print(f"   Firmware URL: {service.firmware_url}")
    is_mock = isinstance(service, MockFirmwareService)
    print(f"   Is Mock: {is_mock}")
    
    if is_mock:
        print("   🚨 ERROR: Still using MockFirmwareService!")
        return False
    else:
        print("   ✅ SUCCESS: Using FirmwareIntegrationService")
    
    # Test 2: TelemetryService
    print("\n2️⃣ Testing TelemetryService:")
    telemetry_is_mock = isinstance(telemetry_service.firmware_service, MockFirmwareService)
    print(f"   TelemetryService using mock: {telemetry_is_mock}")
    
    if telemetry_is_mock:
        print("   🚨 ERROR: TelemetryService still using MockFirmwareService!")
        return False
    else:
        print("   ✅ SUCCESS: TelemetryService using FirmwareIntegrationService")
    
    # Test 3: SafetyService
    print("\n3️⃣ Testing SafetyService:")
    safety_is_mock = isinstance(safety_service.firmware_service, MockFirmwareService)
    print(f"   SafetyService using mock: {safety_is_mock}")
    
    if safety_is_mock:
        print("   🚨 ERROR: SafetyService still using MockFirmwareService!")
        return False
    else:
        print("   ✅ SUCCESS: SafetyService using FirmwareIntegrationService")
    
    # Test 4: Connection validation
    print("\n4️⃣ Testing Firmware Connection:")
    try:
        # Test connection to firmware (will fail if firmware not running)
        connection_valid = await service.validate_firmware_connection()
        print(f"   Connection valid: {connection_valid}")
        
        if connection_valid:
            print("   ✅ SUCCESS: Connected to real firmware!")
        else:
            print("   ⚠️  WARNING: Cannot connect to firmware (expected if firmware not running)")
            print("   ✅ But service is configured correctly for real firmware")
        
    except Exception as e:
        print(f"   ⚠️  WARNING: Connection test failed: {e}")
        print("   ✅ But service is configured correctly for real firmware")
    
    print("\n🎯 SUMMARY:")
    print("=" * 50)
    print("✅ All services now configured for REAL firmware")
    print("✅ No more MockFirmwareService usage")
    print("✅ Issue #108 FIXED!")
    
    return True


if __name__ == "__main__":
    result = asyncio.run(test_firmware_services())
    sys.exit(0 if result else 1)
