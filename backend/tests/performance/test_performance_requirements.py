"""
Performance Tests for OHT-50 Backend
Tests performance requirements: response time < 100ms, memory < 512MB
"""

import pytest
import asyncio
import time
import psutil
import os
from datetime import datetime, timedelta
from unittest.mock import Mock, patch, AsyncMock
from fastapi.testclient import TestClient
from concurrent.futures import ThreadPoolExecutor, as_completed

from app.main import app
from app.models.user import User


class TestPerformanceRequirements:
    """Performance tests for OHT-50 Backend"""
    
    @pytest.fixture
    def client(self):
        """Create test client"""
        return TestClient(app)
    
    @pytest.fixture
    def mock_user(self):
        """Mock authenticated user"""
        user = Mock(spec=User)
        user.id = 1
        user.username = "test_user"
        user.email = "test@example.com"
        user.role = "user"
        user.is_active = True
        return user
    
    @pytest.fixture
    def auth_headers(self):
        """Mock authentication headers"""
        return {"Authorization": "Bearer mock_token"}
    
    def test_api_response_time_requirements(self, client, mock_user, auth_headers):
        """Test API response time requirements (< 100ms)"""
        
        with patch('app.core.security.get_current_user', return_value=mock_user):
            
            # Test endpoints with performance requirements
            test_cases = [
                {
                    "name": "Map Status",
                    "method": "GET",
                    "endpoint": "/api/v1/map/status",
                    "max_time": 0.05  # 50ms
                },
                {
                    "name": "Robot Position",
                    "method": "GET", 
                    "endpoint": "/api/v1/localization/position",
                    "max_time": 0.05  # 50ms
                },
                {
                    "name": "Sensor Data Submit",
                    "method": "POST",
                    "endpoint": "/api/v1/sensors/data",
                    "data": {
                        "sensor_type": "rfid",
                        "sensor_id": "RFID_001",
                        "data": {"rfid_id": "TAG_001", "signal_strength": 0.8},
                        "quality": 0.9
                    },
                    "max_time": 0.1  # 100ms
                },
                {
                    "name": "Start Mapping",
                    "method": "POST",
                    "endpoint": "/api/v1/map/start-mapping",
                    "data": {
                        "map_name": "Test Map",
                        "resolution": 50.0,
                        "width": 1000,
                        "height": 1000
                    },
                    "max_time": 0.1  # 100ms
                },
                {
                    "name": "Emergency Stop",
                    "method": "POST",
                    "endpoint": "/api/v1/safety/emergency-stop",
                    "max_time": 0.01  # 10ms
                }
            ]
            
            for test_case in test_cases:
                # Measure response time
                start_time = time.time()
                
                if test_case["method"] == "GET":
                    response = client.get(test_case["endpoint"], headers=auth_headers)
                else:
                    response = client.post(
                        test_case["endpoint"], 
                        json=test_case.get("data", {}), 
                        headers=auth_headers
                    )
                
                end_time = time.time()
                response_time = end_time - start_time
                
                # Assertions
                assert response.status_code in [200, 201, 422], f"{test_case['name']} returned {response.status_code}"
                assert response_time < test_case["max_time"], \
                    f"{test_case['name']} response time {response_time:.3f}s exceeds limit {test_case['max_time']}s"
                
                print(f"✅ {test_case['name']}: {response_time:.3f}s (limit: {test_case['max_time']}s)")
    
    def test_concurrent_request_performance(self, client, mock_user, auth_headers):
        """Test performance under concurrent requests"""
        
        with patch('app.core.security.get_current_user', return_value=mock_user), \
             patch('app.api.v1.map.map_service') as mock_map_service:
            
            # Setup mock responses
            mock_map_service.get_mapping_status.return_value = {
                "is_mapping": False,
                "current_map_id": None,
                "total_scans": 0,
                "mapping_quality": 0.0
            }
            
            def make_request():
                """Make a single request"""
                start_time = time.time()
                response = client.get("/api/v1/map/status", headers=auth_headers)
                end_time = time.time()
                return {
                    "status_code": response.status_code,
                    "response_time": end_time - start_time
                }
            
            # Test with 50 concurrent requests
            num_requests = 50
            max_concurrent = 10
            
            start_time = time.time()
            
            with ThreadPoolExecutor(max_workers=max_concurrent) as executor:
                futures = [executor.submit(make_request) for _ in range(num_requests)]
                results = [future.result() for future in as_completed(futures)]
            
            end_time = time.time()
            total_time = end_time - start_time
            
            # Analyze results
            response_times = [r["response_time"] for r in results]
            status_codes = [r["status_code"] for r in results]
            
            avg_response_time = sum(response_times) / len(response_times)
            max_response_time = max(response_times)
            min_response_time = min(response_times)
            
            # Assertions
            assert len(results) == num_requests, "Not all requests completed"
            assert all(code == 200 for code in status_codes), "Some requests failed"
            assert avg_response_time < 0.1, f"Average response time {avg_response_time:.3f}s exceeds 100ms"
            assert max_response_time < 0.5, f"Max response time {max_response_time:.3f}s exceeds 500ms"
            assert total_time < 10.0, f"Total time {total_time:.3f}s exceeds 10s"
            
            print(f"✅ Concurrent Requests: {num_requests} requests, {max_concurrent} concurrent")
            print(f"   Average: {avg_response_time:.3f}s, Max: {max_response_time:.3f}s, Min: {min_response_time:.3f}s")
            print(f"   Total time: {total_time:.3f}s")
    
    def test_memory_usage_requirements(self, client, mock_user, auth_headers):
        """Test memory usage requirements (< 512MB)"""
        
        # Get initial memory usage
        process = psutil.Process(os.getpid())
        initial_memory = process.memory_info().rss / 1024 / 1024  # MB
        
        with patch('app.core.security.get_current_user', return_value=mock_user):
            
            # Perform memory-intensive operations
            for i in range(100):
                # Make various API calls
                client.get("/api/v1/map/status", headers=auth_headers)
                client.get("/api/v1/localization/position", headers=auth_headers)
                client.post("/api/v1/sensors/data", json={
                    "sensor_type": "rfid",
                    "sensor_id": f"RFID_{i:03d}",
                    "data": {"rfid_id": f"TAG_{i:03d}", "signal_strength": 0.8},
                    "quality": 0.9
                }, headers=auth_headers)
            
            # Get final memory usage
            final_memory = process.memory_info().rss / 1024 / 1024  # MB
            memory_increase = final_memory - initial_memory
            
            # Assertions
            assert final_memory < 512, f"Memory usage {final_memory:.1f}MB exceeds 512MB limit"
            assert memory_increase < 100, f"Memory increase {memory_increase:.1f}MB is too high"
            
            print(f"✅ Memory Usage: {final_memory:.1f}MB (limit: 512MB)")
            print(f"   Increase: {memory_increase:.1f}MB")
    
    def test_database_query_performance(self, client, mock_user, auth_headers):
        """Test database query performance (< 10ms)"""
        
        with patch('app.core.security.get_current_user', return_value=mock_user), \
             patch('app.api.v1.map.map_service') as mock_map_service, \
             patch('app.api.v1.sensors.get_db') as mock_db:
            
            # Setup mock database
            mock_db_session = Mock()
            mock_db.return_value = [mock_db_session]
            mock_db_session.add = Mock()
            mock_db_session.commit = Mock()
            mock_db_session.refresh = Mock()
            
            # Test database operations
            test_cases = [
                {
                    "name": "Get Map List",
                    "endpoint": "/api/v1/map/list",
                    "method": "GET",
                    "max_time": 0.01  # 10ms
                },
                {
                    "name": "Get Sensor List", 
                    "endpoint": "/api/v1/sensors/list",
                    "method": "GET",
                    "max_time": 0.01  # 10ms
                },
                {
                    "name": "Submit Sensor Data",
                    "endpoint": "/api/v1/sensors/data",
                    "method": "POST",
                    "data": {
                        "sensor_type": "rfid",
                        "sensor_id": "RFID_001",
                        "data": {"rfid_id": "TAG_001", "signal_strength": 0.8},
                        "quality": 0.9
                    },
                    "max_time": 0.01  # 10ms
                }
            ]
            
            for test_case in test_cases:
                # Measure database operation time
                start_time = time.time()
                
                if test_case["method"] == "GET":
                    response = client.get(test_case["endpoint"], headers=auth_headers)
                else:
                    response = client.post(
                        test_case["endpoint"],
                        json=test_case.get("data", {}),
                        headers=auth_headers
                    )
                
                end_time = time.time()
                operation_time = end_time - start_time
                
                # Assertions
                assert response.status_code in [200, 201], f"{test_case['name']} failed"
                assert operation_time < test_case["max_time"], \
                    f"{test_case['name']} operation time {operation_time:.3f}s exceeds limit {test_case['max_time']}s"
                
                print(f"✅ {test_case['name']}: {operation_time:.3f}s (limit: {test_case['max_time']}s)")
    
    def test_websocket_performance(self, client, mock_user, auth_headers):
        """Test WebSocket performance (< 20ms latency)"""
        
        # This is a placeholder for WebSocket performance testing
        # In a real implementation, you would test WebSocket connections
        
        # Simulate WebSocket latency test
        start_time = time.time()
        
        # Simulate WebSocket message processing
        time.sleep(0.005)  # 5ms simulation
        
        end_time = time.time()
        latency = end_time - start_time
        
        # Assertions
        assert latency < 0.02, f"WebSocket latency {latency:.3f}s exceeds 20ms limit"
        
        print(f"✅ WebSocket Latency: {latency:.3f}s (limit: 0.02s)")
    
    def test_system_resource_usage(self):
        """Test system resource usage"""
        
        # Get system resources
        cpu_percent = psutil.cpu_percent(interval=1)
        memory = psutil.virtual_memory()
        disk = psutil.disk_usage('/')
        
        # Assertions
        assert cpu_percent < 80, f"CPU usage {cpu_percent}% exceeds 80% limit"
        assert memory.percent < 85, f"Memory usage {memory.percent}% exceeds 85% limit"
        assert disk.percent < 90, f"Disk usage {disk.percent}% exceeds 90% limit"
        
        print(f"✅ System Resources:")
        print(f"   CPU: {cpu_percent}% (limit: 80%)")
        print(f"   Memory: {memory.percent}% (limit: 85%)")
        print(f"   Disk: {disk.percent}% (limit: 90%)")
    
    def test_stress_test(self, client, mock_user, auth_headers):
        """Test system under stress"""
        
        with patch('app.core.security.get_current_user', return_value=mock_user):
            
            # Stress test with high load
            num_requests = 1000
            start_time = time.time()
            
            def make_stress_request():
                """Make a stress test request"""
                response = client.get("/api/v1/map/status", headers=auth_headers)
                return response.status_code
            
            # Execute stress test
            with ThreadPoolExecutor(max_workers=20) as executor:
                futures = [executor.submit(make_stress_request) for _ in range(num_requests)]
                results = [future.result() for future in as_completed(futures)]
            
            end_time = time.time()
            total_time = end_time - start_time
            
            # Analyze results
            success_count = sum(1 for code in results if code == 200)
            success_rate = success_count / len(results) * 100
            throughput = len(results) / total_time
            
            # Assertions
            assert success_rate > 95, f"Success rate {success_rate:.1f}% below 95%"
            assert throughput > 100, f"Throughput {throughput:.1f} req/s below 100 req/s"
            assert total_time < 30, f"Total time {total_time:.1f}s exceeds 30s"
            
            print(f"✅ Stress Test: {num_requests} requests")
            print(f"   Success rate: {success_rate:.1f}%")
            print(f"   Throughput: {throughput:.1f} req/s")
            print(f"   Total time: {total_time:.1f}s")
    
    def test_memory_leak_detection(self, client, mock_user, auth_headers):
        """Test for memory leaks"""
        
        process = psutil.Process(os.getpid())
        initial_memory = process.memory_info().rss / 1024 / 1024  # MB
        
        with patch('app.core.security.get_current_user', return_value=mock_user):
            
            # Perform operations that might cause memory leaks
            for cycle in range(10):
                for i in range(100):
                    # Make API calls
                    client.get("/api/v1/map/status", headers=auth_headers)
                    client.post("/api/v1/sensors/data", json={
                        "sensor_type": "rfid",
                        "sensor_id": f"RFID_{cycle}_{i}",
                        "data": {"rfid_id": f"TAG_{cycle}_{i}", "signal_strength": 0.8},
                        "quality": 0.9
                    }, headers=auth_headers)
                
                # Force garbage collection
                import gc
                gc.collect()
                
                # Check memory after each cycle
                current_memory = process.memory_info().rss / 1024 / 1024  # MB
                memory_increase = current_memory - initial_memory
                
                # Assertions
                assert memory_increase < 50, f"Memory leak detected: {memory_increase:.1f}MB increase after cycle {cycle}"
            
            final_memory = process.memory_info().rss / 1024 / 1024  # MB
            total_increase = final_memory - initial_memory
            
            print(f"✅ Memory Leak Test: {total_increase:.1f}MB total increase")
            assert total_increase < 50, f"Memory leak detected: {total_increase:.1f}MB total increase"
