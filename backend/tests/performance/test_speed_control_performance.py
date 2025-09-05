"""
Performance tests for Speed Control System
Tests speed control performance under various load conditions
"""

import pytest
import asyncio
import time
import statistics
from concurrent.futures import ThreadPoolExecutor
from httpx import AsyncClient

from app.main import app
from app.services.speed_control import speed_controller


class TestSpeedControlPerformance:
    """Test Speed Control performance requirements"""
    
    @pytest.fixture
    def client(self):
        """Create test client with authentication override"""
        from fastapi.testclient import TestClient
        from app.api.deps import get_current_user
        from app.schemas.user import UserResponse, UserRole, UserStatus
        from datetime import datetime
        
        def override_get_current_user():
            return UserResponse(
                id=1,
                username="test_admin",
                email="admin@test.com",
                full_name="Test Admin",
                role=UserRole.ADMIN,
                status=UserStatus.ACTIVE,
                created_at=datetime.now(),
                updated_at=datetime.now()
            )
        
        app.dependency_overrides[get_current_user] = override_get_current_user
        client = TestClient(app)
        yield client
        app.dependency_overrides.clear()
    
    @pytest.fixture
    def auth_headers(self):
        """Create authentication headers for admin user"""
        return {
            "Authorization": "Bearer test_token",
            "X-User-Role": "admin",
            "X-User-Id": "test_admin"
        }
    
    def test_single_speed_command_performance(self, client, auth_headers):
        """Test single speed command performance - target < 5ms"""
        response_times = []
        
        # Execute 100 speed commands and measure response times
        for i in range(100):
            start_time = time.time()
            
            response = client.post(
                "/api/v1/speed-control/set-speed",
                json={
                    "target_speed": 0.5 + (i * 0.01),
                    "mode": "manual",
                    "safety_override": False
                },
                headers=auth_headers
            )
            
            end_time = time.time()
            response_time_ms = (end_time - start_time) * 1000
            response_times.append(response_time_ms)
            
            assert response.status_code == 200
            assert response.json()["success"] is True
        
        # Calculate performance metrics
        avg_response_time = statistics.mean(response_times)
        max_response_time = max(response_times)
        min_response_time = min(response_times)
        p95_response_time = statistics.quantiles(response_times, n=20)[18]  # 95th percentile
        
        # Performance assertions
        assert avg_response_time < 5.0, f"Average response time {avg_response_time:.2f}ms exceeds 5ms target"
        assert max_response_time < 10.0, f"Max response time {max_response_time:.2f}ms exceeds 10ms limit"
        assert p95_response_time < 7.0, f"95th percentile response time {p95_response_time:.2f}ms exceeds 7ms limit"
        
        print(f"Speed Control Performance Metrics:")
        print(f"  Average: {avg_response_time:.2f}ms")
        print(f"  Min: {min_response_time:.2f}ms")
        print(f"  Max: {max_response_time:.2f}ms")
        print(f"  95th percentile: {p95_response_time:.2f}ms")
    
    def test_emergency_stop_performance(self, client, auth_headers):
        """Test emergency stop performance - target < 5ms"""
        response_times = []
        
        # Execute 50 emergency stops and measure response times
        for i in range(50):
            # First set some speed
            client.post(
                "/api/v1/speed-control/set-speed",
                json={
                    "target_speed": 1.5,
                    "mode": "manual",
                    "safety_override": False
                },
                headers=auth_headers
            )
            
            # Then measure emergency stop time
            start_time = time.time()
            
            response = client.post(
                "/api/v1/speed-control/emergency-stop",
                headers=auth_headers
            )
            
            end_time = time.time()
            response_time_ms = (end_time - start_time) * 1000
            response_times.append(response_time_ms)
            
            assert response.status_code == 200
            assert response.json()["success"] is True
            assert response.json()["current_speed"] == 0.0
        
        # Calculate performance metrics
        avg_response_time = statistics.mean(response_times)
        max_response_time = max(response_times)
        
        # Emergency stop should be very fast
        assert avg_response_time < 3.0, f"Average emergency stop time {avg_response_time:.2f}ms exceeds 3ms target"
        assert max_response_time < 5.0, f"Max emergency stop time {max_response_time:.2f}ms exceeds 5ms limit"
        
        print(f"Emergency Stop Performance Metrics:")
        print(f"  Average: {avg_response_time:.2f}ms")
        print(f"  Max: {max_response_time:.2f}ms")
    
    def test_concurrent_speed_commands(self, client, auth_headers):
        """Test concurrent speed commands performance"""
        def execute_speed_command(speed_value):
            """Execute a single speed command"""
            start_time = time.time()
            
            response = client.post(
                "/api/v1/speed-control/set-speed",
                json={
                    "target_speed": speed_value,
                    "mode": "manual",
                    "safety_override": False
                },
                headers=auth_headers
            )
            
            end_time = time.time()
            response_time_ms = (end_time - start_time) * 1000
            
            return {
                "status_code": response.status_code,
                "response_time_ms": response_time_ms,
                "success": response.json().get("success", False)
            }
        
        # Execute 20 concurrent speed commands
        with ThreadPoolExecutor(max_workers=10) as executor:
            speeds = [0.5 + (i * 0.1) for i in range(20)]
            futures = [executor.submit(execute_speed_command, speed) for speed in speeds]
            results = [future.result() for future in futures]
        
        # Analyze results
        response_times = [r["response_time_ms"] for r in results]
        success_count = sum(1 for r in results if r["success"])
        
        avg_response_time = statistics.mean(response_times)
        max_response_time = max(response_times)
        
        # All commands should succeed
        assert success_count == 20, f"Only {success_count}/20 commands succeeded"
        
        # Performance should still be good under concurrent load
        assert avg_response_time < 8.0, f"Average concurrent response time {avg_response_time:.2f}ms exceeds 8ms target"
        assert max_response_time < 15.0, f"Max concurrent response time {max_response_time:.2f}ms exceeds 15ms limit"
        
        print(f"Concurrent Speed Commands Performance:")
        print(f"  Commands executed: {len(results)}")
        print(f"  Success rate: {success_count/len(results)*100:.1f}%")
        print(f"  Average response time: {avg_response_time:.2f}ms")
        print(f"  Max response time: {max_response_time:.2f}ms")
    
    def test_high_frequency_speed_commands(self, client, auth_headers):
        """Test high frequency speed commands"""
        response_times = []
        start_time = time.time()
        
        # Execute 200 speed commands as fast as possible
        for i in range(200):
            cmd_start = time.time()
            
            response = client.post(
                "/api/v1/speed-control/set-speed",
                json={
                    "target_speed": 0.5 + (i % 10) * 0.1,  # Cycle through speeds
                    "mode": "manual",
                    "safety_override": False
                },
                headers=auth_headers
            )
            
            cmd_end = time.time()
            response_time_ms = (cmd_end - cmd_start) * 1000
            response_times.append(response_time_ms)
            
            assert response.status_code == 200
            assert response.json()["success"] is True
        
        total_time = time.time() - start_time
        
        # Calculate performance metrics
        avg_response_time = statistics.mean(response_times)
        throughput = 200 / total_time  # commands per second
        
        # Performance assertions
        assert avg_response_time < 10.0, f"Average response time {avg_response_time:.2f}ms exceeds 10ms target"
        assert throughput > 50, f"Throughput {throughput:.1f} commands/sec below 50 target"
        
        print(f"High Frequency Speed Commands Performance:")
        print(f"  Total commands: 200")
        print(f"  Total time: {total_time:.2f}s")
        print(f"  Throughput: {throughput:.1f} commands/sec")
        print(f"  Average response time: {avg_response_time:.2f}ms")
    
    def test_mixed_workload_performance(self, client, auth_headers):
        """Test performance with mixed workload (speed commands + status checks)"""
        response_times = []
        
        # Mix of different operations
        operations = []
        for i in range(50):
            operations.append(("set_speed", 0.5 + (i * 0.02)))
            operations.append(("get_status", None))
            operations.append(("emergency_stop", None))
        
        # Execute mixed operations
        for op_type, speed_value in operations:
            start_time = time.time()
            
            if op_type == "set_speed":
                response = client.post(
                    "/api/v1/speed-control/set-speed",
                    json={
                        "target_speed": speed_value,
                        "mode": "manual",
                        "safety_override": False
                    },
                    headers=auth_headers
                )
            elif op_type == "get_status":
                response = client.get(
                    "/api/v1/speed-control/status",
                    headers=auth_headers
                )
            elif op_type == "emergency_stop":
                response = client.post(
                    "/api/v1/speed-control/emergency-stop",
                    headers=auth_headers
                )
            
            end_time = time.time()
            response_time_ms = (end_time - start_time) * 1000
            response_times.append(response_time_ms)
            
            assert response.status_code == 200
        
        # Calculate performance metrics
        avg_response_time = statistics.mean(response_times)
        max_response_time = max(response_times)
        
        # Mixed workload should still perform well
        assert avg_response_time < 8.0, f"Average mixed workload response time {avg_response_time:.2f}ms exceeds 8ms target"
        assert max_response_time < 15.0, f"Max mixed workload response time {max_response_time:.2f}ms exceeds 15ms limit"
        
        print(f"Mixed Workload Performance:")
        print(f"  Operations executed: {len(operations)}")
        print(f"  Average response time: {avg_response_time:.2f}ms")
        print(f"  Max response time: {max_response_time:.2f}ms")
    
    def test_memory_usage_under_load(self, client, auth_headers):
        """Test memory usage under sustained load"""
        import psutil
        import os
        
        # Get initial memory usage
        process = psutil.Process(os.getpid())
        initial_memory = process.memory_info().rss / 1024 / 1024  # MB
        
        # Execute sustained load
        for i in range(1000):
            response = client.post(
                "/api/v1/speed-control/set-speed",
                json={
                    "target_speed": 0.5 + (i % 20) * 0.1,
                    "mode": "manual",
                    "safety_override": False
                },
                headers=auth_headers
            )
            assert response.status_code == 200
        
        # Get final memory usage
        final_memory = process.memory_info().rss / 1024 / 1024  # MB
        memory_increase = final_memory - initial_memory
        
        # Memory increase should be reasonable
        assert memory_increase < 50, f"Memory increase {memory_increase:.1f}MB exceeds 50MB limit"
        
        print(f"Memory Usage Under Load:")
        print(f"  Initial memory: {initial_memory:.1f}MB")
        print(f"  Final memory: {final_memory:.1f}MB")
        print(f"  Memory increase: {memory_increase:.1f}MB")
    
    def test_performance_metrics_accuracy(self, client, auth_headers):
        """Test that performance metrics are accurate"""
        # Execute some commands
        for i in range(10):
            response = client.post(
                "/api/v1/speed-control/set-speed",
                json={
                    "target_speed": 0.5 + (i * 0.1),
                    "mode": "manual",
                    "safety_override": False
                },
                headers=auth_headers
            )
            assert response.status_code == 200
        
        # Get performance metrics
        response = client.get(
            "/api/v1/speed-control/performance",
            headers=auth_headers
        )
        assert response.status_code == 200
        
        metrics = response.json()["data"]
        
        # Verify metrics accuracy
        assert metrics["total_commands"] >= 10
        assert metrics["avg_response_time_ms"] > 0
        assert metrics["min_response_time_ms"] > 0
        assert metrics["max_response_time_ms"] > 0
        assert metrics["response_time_target_ms"] == 5.0
        assert isinstance(metrics["target_met"], bool)
        
        # All commands should meet performance target
        assert metrics["target_met"] is True
        
        print(f"Performance Metrics Accuracy:")
        print(f"  Total commands: {metrics['total_commands']}")
        print(f"  Average response time: {metrics['avg_response_time_ms']:.2f}ms")
        print(f"  Min response time: {metrics['min_response_time_ms']:.2f}ms")
        print(f"  Max response time: {metrics['max_response_time_ms']:.2f}ms")
        print(f"  Target met: {metrics['target_met']}")


class TestSpeedControlStressTest:
    """Stress tests for Speed Control system"""
    
    @pytest.fixture
    def client(self):
        """Create test client with authentication override"""
        from fastapi.testclient import TestClient
        from app.api.deps import get_current_user
        from app.schemas.user import UserResponse, UserRole, UserStatus
        from datetime import datetime
        
        def override_get_current_user():
            return UserResponse(
                id=1,
                username="test_admin",
                email="admin@test.com",
                full_name="Test Admin",
                role=UserRole.ADMIN,
                status=UserStatus.ACTIVE,
                created_at=datetime.now(),
                updated_at=datetime.now()
            )
        
        app.dependency_overrides[get_current_user] = override_get_current_user
        client = TestClient(app)
        yield client
        app.dependency_overrides.clear()
    
    @pytest.fixture
    def auth_headers(self):
        """Create authentication headers for admin user"""
        return {
            "Authorization": "Bearer test_token",
            "X-User-Role": "admin",
            "X-User-Id": "test_admin"
        }
    
    def test_stress_test_1000_commands(self, client, auth_headers):
        """Stress test with 1000 speed commands"""
        start_time = time.time()
        success_count = 0
        
        for i in range(1000):
            response = client.post(
                "/api/v1/speed-control/set-speed",
                json={
                    "target_speed": 0.5 + (i % 50) * 0.02,
                    "mode": "manual",
                    "safety_override": False
                },
                headers=auth_headers
            )
            
            if response.status_code == 200 and response.json()["success"]:
                success_count += 1
        
        total_time = time.time() - start_time
        throughput = 1000 / total_time
        
        # Stress test should maintain high success rate
        assert success_count >= 990, f"Success rate {success_count}/1000 below 99%"
        assert throughput > 100, f"Throughput {throughput:.1f} commands/sec below 100 target"
        
        print(f"Stress Test Results (1000 commands):")
        print(f"  Success rate: {success_count}/1000 ({success_count/10:.1f}%)")
        print(f"  Total time: {total_time:.2f}s")
        print(f"  Throughput: {throughput:.1f} commands/sec")
    
    def test_rapid_emergency_stops(self, client, auth_headers):
        """Test rapid emergency stops"""
        success_count = 0
        
        for i in range(100):
            # Set speed first
            client.post(
                "/api/v1/speed-control/set-speed",
                json={
                    "target_speed": 1.5,
                    "mode": "manual",
                    "safety_override": False
                },
                headers=auth_headers
            )
            
            # Then emergency stop
            response = client.post(
                "/api/v1/speed-control/emergency-stop",
                headers=auth_headers
            )
            
            if response.status_code == 200 and response.json()["success"]:
                success_count += 1
        
        # All emergency stops should succeed
        assert success_count == 100, f"Only {success_count}/100 emergency stops succeeded"
        
        print(f"Rapid Emergency Stops Test:")
        print(f"  Success rate: {success_count}/100 ({success_count}%)")
