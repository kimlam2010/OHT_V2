#!/usr/bin/env python3
"""
Test Script for Firmware Integration - OHT-50 Backend

This script tests the firmware integration functionality
without requiring a full backend server.
"""

import asyncio
import logging
import sys
import os

# Add backend to path
sys.path.insert(0, os.path.join(os.path.dirname(__file__), '.'))

# Set environment variables
os.environ['USE_MOCK_FIRMWARE'] = 'false'
os.environ['TESTING'] = 'false'
os.environ['ENVIRONMENT'] = 'development'

# Configure logging
logging.basicConfig(
    level=logging.INFO,
    format='%(asctime)s - %(name)s - %(levelname)s - %(message)s'
)
logger = logging.getLogger(__name__)


async def test_fw_client():
    """Test FW Client functionality"""
    try:
        from app.lib.fw_client import FWClient, FWConfig
        
        logger.info("🧪 Testing FW Client...")
        
        # Create FW client
        config = FWConfig(
            host="localhost",
            http_port=8080,
            ws_port=8081,
            timeout=5.0
        )
        
        client = FWClient(config)
        logger.info(f"✅ FW Client created: {client.http_url}")
        
        # Test connection (will fail if firmware not running)
        try:
            connected = await client.connect()
            if connected:
                logger.info("✅ FW Client connected successfully")
                
                # Test system health
                health = await client.get_system_health()
                logger.info(f"📊 System health: {health}")
                
                # Test system status
                status = await client.get_system_status()
                logger.info(f"📊 System status: {status}")
                
                # Test modules
                modules = await client.get_modules()
                logger.info(f"📊 Modules found: {len(modules)}")
                
            else:
                logger.warning("⚠️ FW Client connection failed (expected if firmware not running)")
                
        except Exception as e:
            logger.warning(f"⚠️ FW Client connection test failed: {e}")
        
        # Cleanup
        await client.disconnect()
        logger.info("✅ FW Client test completed")
        
    except Exception as e:
        logger.error(f"❌ FW Client test failed: {e}")


async def test_firmware_service():
    """Test Firmware Integration Service"""
    try:
        from app.services.firmware_integration_service import FirmwareIntegrationService
        
        logger.info("🧪 Testing Firmware Integration Service...")
        
        # Create service
        service = FirmwareIntegrationService()
        logger.info(f"✅ Firmware Integration Service created: {service.firmware_url}")
        
        # Test initialization
        try:
            initialized = await service.initialize()
            if initialized:
                logger.info("✅ Firmware Integration Service initialized successfully")
                
                # Test system status
                status = await service.get_system_status()
                logger.info(f"📊 System status: {status}")
                
                # Test modules
                modules = await service.get_modules()
                logger.info(f"📊 Modules found: {len(modules)}")
                
                # Test telemetry
                telemetry = await service.get_telemetry_data()
                logger.info(f"📊 Telemetry data: {telemetry.get('success', False)}")
                
            else:
                logger.warning("⚠️ Firmware Integration Service initialization failed (expected if firmware not running)")
                
        except Exception as e:
            logger.warning(f"⚠️ Firmware Integration Service test failed: {e}")
        
        # Cleanup
        await service.shutdown()
        logger.info("✅ Firmware Integration Service test completed")
        
    except Exception as e:
        logger.error(f"❌ Firmware Integration Service test failed: {e}")


async def test_config():
    """Test configuration"""
    try:
        from app.config import settings
        
        logger.info("🧪 Testing Configuration...")
        
        logger.info(f"✅ Firmware URL: {settings.firmware_url}")
        logger.info(f"✅ Firmware WebSocket URL: {settings.firmware_websocket_url}")
        logger.info(f"✅ Use Mock Firmware: {settings.use_mock_firmware}")
        logger.info(f"✅ Environment: {settings.environment}")
        
        logger.info("✅ Configuration test completed")
        
    except Exception as e:
        logger.error(f"❌ Configuration test failed: {e}")


async def main():
    """Main test function"""
    logger.info("🚀 Starting Firmware Integration Tests...")
    
    # Test configuration
    await test_config()
    
    # Test FW Client
    await test_fw_client()
    
    # Test Firmware Integration Service
    await test_firmware_service()
    
    logger.info("🎉 All tests completed!")


if __name__ == "__main__":
    asyncio.run(main())
