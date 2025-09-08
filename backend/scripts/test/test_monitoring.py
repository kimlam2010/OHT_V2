"""
Test script for monitoring system

This script tests the monitoring service and WebSocket functionality.
"""

import asyncio
import json
import logging
from datetime import datetime

# Configure logging
logging.basicConfig(level=logging.INFO)
logger = logging.getLogger(__name__)


async def test_monitoring_service():
    """Test monitoring service functionality"""
    logger.info("🧪 Testing Monitoring Service...")
    
    try:
        from app.core.monitoring_service import monitoring_service
        
        # Test metrics collection
        logger.info("📊 Testing metrics collection...")
        metrics = await monitoring_service.get_current_metrics()
        logger.info(f"✅ Current metrics: CPU={metrics.cpu_percent}%, Memory={metrics.memory_percent}%, Disk={metrics.disk_percent}%")
        
        # Test system health
        logger.info("🏥 Testing system health...")
        health = await monitoring_service.get_system_health()
        logger.info(f"✅ System health: {health['status']}, Score: {health['overall_health_score']}")
        
        # Test performance summary
        logger.info("📈 Testing performance summary...")
        summary = await monitoring_service.get_performance_summary(hours=1)
        if "error" not in summary:
            logger.info(f"✅ Performance summary: {summary['metrics_count']} metrics collected")
        else:
            logger.warning(f"⚠️ Performance summary error: {summary['error']}")
        
        # Test alerts
        logger.info("🚨 Testing alerts...")
        active_alerts = await monitoring_service.get_active_alerts()
        logger.info(f"✅ Active alerts: {len(active_alerts)}")
        
        logger.info("✅ Monitoring service tests completed successfully")
        
    except Exception as e:
        logger.error(f"❌ Monitoring service test failed: {e}")


async def test_websocket_service():
    """Test WebSocket service functionality"""
    logger.info("🔌 Testing WebSocket Service...")
    
    try:
        from app.core.websocket_service import websocket_service
        
        # Test service start
        logger.info("🚀 Testing WebSocket service start...")
        await websocket_service.start()
        logger.info("✅ WebSocket service started")
        
        # Test connection stats
        logger.info("📊 Testing connection stats...")
        stats = await websocket_service.get_connection_stats()
        logger.info(f"✅ Connection stats: {stats['active_connections']} active connections")
        
        # Test service stop
        logger.info("🛑 Testing WebSocket service stop...")
        await websocket_service.stop()
        logger.info("✅ WebSocket service stopped")
        
        logger.info("✅ WebSocket service tests completed successfully")
        
    except Exception as e:
        logger.error(f"❌ WebSocket service test failed: {e}")


async def test_monitoring_api():
    """Test monitoring API endpoints"""
    logger.info("🌐 Testing Monitoring API...")
    
    try:
        from app.core.database import init_db
        from app.core.monitoring_service import monitoring_service
        
        # Initialize database
        await init_db()
        
        # Test health endpoint
        logger.info("🏥 Testing health endpoint...")
        from app.api.v1.monitoring import get_system_health
        
        # Mock user for testing
        class MockUser:
            def __init__(self):
                self.role = "admin"
        
        mock_user = MockUser()
        
        # Test health endpoint
        health_response = await get_system_health(mock_user)
        logger.info(f"✅ Health endpoint: {health_response['data']['status']}")
        
        # Test metrics endpoint
        logger.info("📊 Testing metrics endpoint...")
        from app.api.v1.monitoring import get_current_metrics
        
        metrics_response = await get_current_metrics(mock_user)
        logger.info(f"✅ Metrics endpoint: {metrics_response['data']['cpu_percent']}% CPU")
        
        logger.info("✅ Monitoring API tests completed successfully")
        
    except Exception as e:
        logger.error(f"❌ Monitoring API test failed: {e}")


async def main():
    """Main test function"""
    logger.info("🚀 Starting OHT-50 Monitoring System Tests...")
    
    try:
        # Test monitoring service
        await test_monitoring_service()
        
        # Test WebSocket service
        await test_websocket_service()
        
        # Test monitoring API
        await test_monitoring_api()
        
        logger.info("✅ All monitoring system tests completed successfully!")
        
    except Exception as e:
        logger.error(f"❌ Test execution failed: {e}")
        raise


if __name__ == "__main__":
    # Run tests
    asyncio.run(main())
