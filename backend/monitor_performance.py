#!/usr/bin/env python3
"""
Performance Monitoring Script - OHT-50 Backend

This script monitors the performance and error rates
of the Backend-Firmware integration.
"""

import asyncio
import logging
import sys
import os
import time
from typing import Dict, Any
from datetime import datetime, timezone

# Add backend to path
sys.path.insert(0, os.path.join(os.path.dirname(__file__), '.'))

# Set environment variables
os.environ['USE_MOCK_FIRMWARE'] = 'false'
os.environ['TESTING'] = 'false'
os.environ['ENVIRONMENT'] = 'development'

import aiohttp
from app.lib.fw_client import FWClient, FWConfig

# Configure logging
logging.basicConfig(
    level=logging.INFO,
    format='%(asctime)s - %(name)s - %(levelname)s - %(message)s'
)
logger = logging.getLogger(__name__)


class PerformanceMonitor:
    """Performance monitoring class"""
    
    def __init__(self):
        self.stats = {
            'total_requests': 0,
            'successful_requests': 0,
            'failed_requests': 0,
            'response_times': [],
            'error_counts': {},
            'start_time': datetime.now(timezone.utc)
        }
        
    def record_request(self, success: bool, response_time: float, error: str = None):
        """Record request statistics"""
        self.stats['total_requests'] += 1
        
        if success:
            self.stats['successful_requests'] += 1
        else:
            self.stats['failed_requests'] += 1
            if error:
                self.stats['error_counts'][error] = self.stats['error_counts'].get(error, 0) + 1
        
        self.stats['response_times'].append(response_time)
        
        # Keep only last 100 response times
        if len(self.stats['response_times']) > 100:
            self.stats['response_times'] = self.stats['response_times'][-100:]
    
    def get_stats(self) -> Dict[str, Any]:
        """Get current statistics"""
        response_times = self.stats['response_times']
        
        stats = {
            'total_requests': self.stats['total_requests'],
            'successful_requests': self.stats['successful_requests'],
            'failed_requests': self.stats['failed_requests'],
            'success_rate': (self.stats['successful_requests'] / max(1, self.stats['total_requests'])) * 100,
            'uptime_seconds': (datetime.now(timezone.utc) - self.stats['start_time']).total_seconds(),
            'error_counts': self.stats['error_counts']
        }
        
        if response_times:
            stats['response_time'] = {
                'avg': sum(response_times) / len(response_times),
                'min': min(response_times),
                'max': max(response_times),
                'latest': response_times[-1] if response_times else 0
            }
        else:
            stats['response_time'] = {'avg': 0, 'min': 0, 'max': 0, 'latest': 0}
        
        return stats


async def test_backend_endpoints():
    """Test Backend endpoints"""
    monitor = PerformanceMonitor()
    endpoints = [
        ("GET", "http://localhost:8000/health"),
        ("GET", "http://localhost:8000/api/v1/fw/system/status"),
        ("GET", "http://localhost:8000/api/v1/fw/system/health"),
        ("GET", "http://localhost:8000/api/v1/fw/modules"),
        ("GET", "http://localhost:8000/api/v1/communication/status")
    ]
    
    async with aiohttp.ClientSession() as session:
        for i in range(10):  # Test 10 rounds
            logger.info(f"🧪 Testing round {i+1}/10...")
            
            for method, url in endpoints:
                start_time = time.time()
                success = False
                error = None
                
                try:
                    async with session.get(url, timeout=aiohttp.ClientTimeout(total=5)) as response:
                        response_time = (time.time() - start_time) * 1000  # ms
                        
                        if response.status == 200:
                            success = True
                            logger.debug(f"✅ {method} {url}: {response.status} ({response_time:.1f}ms)")
                        else:
                            error = f"HTTP {response.status}"
                            logger.warning(f"⚠️ {method} {url}: {response.status} ({response_time:.1f}ms)")
                        
                        monitor.record_request(success, response_time, error)
                        
                except Exception as e:
                    response_time = (time.time() - start_time) * 1000
                    error = str(e)
                    logger.error(f"❌ {method} {url}: {error} ({response_time:.1f}ms)")
                    monitor.record_request(False, response_time, error)
                
                await asyncio.sleep(0.1)  # Small delay between requests
            
            await asyncio.sleep(1)  # Wait between rounds
    
    return monitor.get_stats()


async def test_firmware_client():
    """Test firmware client directly"""
    monitor = PerformanceMonitor()
    
    config = FWConfig(
        host="localhost",
        http_port=8080,
        ws_port=8081,
        timeout=5.0
    )
    
    client = FWClient(config)
    
    try:
        # Test connection
        start_time = time.time()
        connected = await client.connect()
        response_time = (time.time() - start_time) * 1000
        
        if connected:
            logger.info(f"✅ FW Client connected ({response_time:.1f}ms)")
            monitor.record_request(True, response_time)
            
            # Test endpoints
            tests = [
                ("health", lambda: client.get('/health')),
                ("system_status", lambda: client.get_system_status()),
                ("modules", lambda: client.get_modules())
            ]
            
            for test_name, test_func in tests:
                start_time = time.time()
                try:
                    result = await test_func()
                    response_time = (time.time() - start_time) * 1000
                    
                    if isinstance(result, dict) and result.get('success', False):
                        logger.info(f"✅ {test_name}: success ({response_time:.1f}ms)")
                        monitor.record_request(True, response_time)
                    else:
                        logger.warning(f"⚠️ {test_name}: failed ({response_time:.1f}ms)")
                        monitor.record_request(False, response_time, f"{test_name}_failed")
                        
                except Exception as e:
                    response_time = (time.time() - start_time) * 1000
                    logger.error(f"❌ {test_name}: {e} ({response_time:.1f}ms)")
                    monitor.record_request(False, response_time, str(e))
                
                await asyncio.sleep(0.1)
        
        else:
            logger.error("❌ FW Client connection failed")
            monitor.record_request(False, response_time, "connection_failed")
    
    finally:
        await client.disconnect()
    
    return monitor.get_stats()


async def main():
    """Main monitoring function"""
    logger.info("🚀 Starting Performance Monitoring...")
    
    # Test Backend endpoints
    logger.info("🧪 Testing Backend endpoints...")
    backend_stats = await test_backend_endpoints()
    
    # Test firmware client
    logger.info("🧪 Testing firmware client...")
    fw_stats = await test_firmware_client()
    
    # Print results
    logger.info("📊 Performance Monitoring Results:")
    logger.info("=" * 50)
    
    logger.info("🔗 Backend Endpoints:")
    logger.info(f"  Total Requests: {backend_stats['total_requests']}")
    logger.info(f"  Success Rate: {backend_stats['success_rate']:.1f}%")
    logger.info(f"  Response Time: {backend_stats['response_time']['avg']:.1f}ms avg, {backend_stats['response_time']['max']:.1f}ms max")
    if backend_stats['error_counts']:
        logger.info(f"  Errors: {backend_stats['error_counts']}")
    
    logger.info("🔌 Firmware Client:")
    logger.info(f"  Total Requests: {fw_stats['total_requests']}")
    logger.info(f"  Success Rate: {fw_stats['success_rate']:.1f}%")
    logger.info(f"  Response Time: {fw_stats['response_time']['avg']:.1f}ms avg, {fw_stats['response_time']['max']:.1f}ms max")
    if fw_stats['error_counts']:
        logger.info(f"  Errors: {fw_stats['error_counts']}")
    
    # Overall assessment
    overall_success_rate = (backend_stats['success_rate'] + fw_stats['success_rate']) / 2
    overall_avg_response = (backend_stats['response_time']['avg'] + fw_stats['response_time']['avg']) / 2
    
    logger.info("🎯 Overall Assessment:")
    logger.info(f"  Overall Success Rate: {overall_success_rate:.1f}%")
    logger.info(f"  Overall Response Time: {overall_avg_response:.1f}ms")
    
    if overall_success_rate >= 80 and overall_avg_response < 1000:
        logger.info("✅ Performance: GOOD")
    elif overall_success_rate >= 60 and overall_avg_response < 2000:
        logger.info("⚠️ Performance: ACCEPTABLE")
    else:
        logger.info("❌ Performance: POOR")
    
    logger.info("=" * 50)


if __name__ == "__main__":
    asyncio.run(main())
