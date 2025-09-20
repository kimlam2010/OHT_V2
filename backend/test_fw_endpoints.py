#!/usr/bin/env python3
"""
Test FW Integration Endpoints - OHT-50 Backend

This script tests the firmware integration endpoints
without requiring full backend server.
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


async def test_fw_endpoints():
    """Test FW Integration endpoints"""
    try:
        from app.lib.fw_client import FWClient, FWConfig
        
        logger.info("🧪 Testing FW Integration Endpoints...")
        
        # Create FW client
        config = FWConfig(
            host="localhost",
            http_port=8080,
            ws_port=8081,
            timeout=5.0
        )
        
        client = FWClient(config)
        
        # Test connection
        connected = await client.connect()
        if not connected:
            logger.error("❌ Failed to connect to firmware")
            return False
        
        logger.info("✅ Connected to firmware")
        
        # Test endpoints that should work
        test_results = {}
        
        # 1. Test /health endpoint
        try:
            health = await client.get('/health')
            test_results['health'] = {
                'success': health.get('success', False),
                'status': health.get('status', 'unknown')
            }
            logger.info(f"✅ Health check: {test_results['health']}")
        except Exception as e:
            test_results['health'] = {'success': False, 'error': str(e)}
            logger.error(f"❌ Health check failed: {e}")
        
        # 2. Test /api/v1/system/status
        try:
            status = await client.get_system_status()
            test_results['system_status'] = {
                'success': status.get('success', False),
                'data': status.get('data', {})
            }
            logger.info(f"✅ System status: {test_results['system_status']}")
        except Exception as e:
            test_results['system_status'] = {'success': False, 'error': str(e)}
            logger.error(f"❌ System status failed: {e}")
        
        # 3. Test modules endpoint (may not exist yet)
        try:
            modules = await client.get_modules()
            test_results['modules'] = {
                'success': True,
                'count': len(modules),
                'data': modules
            }
            logger.info(f"✅ Modules: {test_results['modules']}")
        except Exception as e:
            test_results['modules'] = {'success': False, 'error': str(e)}
            logger.warning(f"⚠️ Modules endpoint not available: {e}")
        
        # 4. Test configuration endpoint (may not exist yet)
        try:
            config = await client.get_configuration()
            test_results['configuration'] = {
                'success': config.get('success', False),
                'data': config.get('data', {})
            }
            logger.info(f"✅ Configuration: {test_results['configuration']}")
        except Exception as e:
            test_results['configuration'] = {'success': False, 'error': str(e)}
            logger.warning(f"⚠️ Configuration endpoint not available: {e}")
        
        await client.disconnect()
        
        # Summary
        successful_tests = sum(1 for result in test_results.values() if result.get('success', False))
        total_tests = len(test_results)
        
        logger.info(f"📊 Test Results: {successful_tests}/{total_tests} successful")
        
        for test_name, result in test_results.items():
            status = "✅" if result.get('success', False) else "❌"
            logger.info(f"{status} {test_name}: {result}")
        
        return successful_tests > 0
        
    except Exception as e:
        logger.error(f"❌ FW endpoints test failed: {e}")
        return False


async def main():
    """Main test function"""
    logger.info("🚀 Starting FW Integration Endpoints Tests...")
    
    success = await test_fw_endpoints()
    
    if success:
        logger.info("🎉 FW Integration tests completed successfully!")
        logger.info("✅ Basic firmware communication is working")
        logger.info("⚠️ Some endpoints may not be implemented yet in firmware")
    else:
        logger.error("❌ FW Integration tests failed!")
    
    return success


if __name__ == "__main__":
    asyncio.run(main())
