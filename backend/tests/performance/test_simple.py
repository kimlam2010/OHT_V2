"""
Simple performance test to verify fixes
"""

import time
import asyncio
import pytest
from httpx import AsyncClient

from app.main import create_app
from app.core.security import create_access_token


@pytest.fixture
def app():
    """Create test app"""
    return create_app()


@pytest.fixture
def async_client(app):
    """Create async test client"""
    return AsyncClient(app=app, base_url="http://test")

@pytest.fixture
def auth_headers():
    """Create auth headers for testing"""
    token = create_access_token(data={"sub": "1"})
    return {"Authorization": f"Bearer {token}"}


class TestSimplePerformance:
    """Simple performance tests"""
    
    @pytest.mark.asyncio
    async def test_simple_robot_status(self, async_client, auth_headers):
        """Test simple robot status request"""
        start_time = time.time()
        
        response = await async_client.get("/api/v1/robot/status", headers=auth_headers)
        
        end_time = time.time()
        response_time = (end_time - start_time) * 1000
        
        assert response.status_code == 200
        assert response_time < 500  # Very generous limit
        
        print(f"Simple robot status response time: {response_time:.2f}ms")
    
    @pytest.mark.asyncio
    async def test_simple_concurrent_requests(self, async_client, auth_headers):
        """Test simple concurrent requests"""
        async def make_request():
            try:
                response = await async_client.get("/api/v1/robot/status", headers=auth_headers)
                return response.status_code
            except Exception as e:
                print(f"Request failed: {e}")
                return 500
        
        start_time = time.time()
        
        # Make 5 simple requests
        tasks = [make_request() for _ in range(5)]
        results = await asyncio.gather(*tasks, return_exceptions=True)
        
        end_time = time.time()
        total_time = (end_time - start_time) * 1000
        
        # At least 3 should succeed
        success_count = sum(1 for r in results if r == 200)
        assert success_count >= 3
        
        print(f"Simple concurrent test - Success: {success_count}/5, Time: {total_time:.2f}ms")
    
    @pytest.mark.asyncio
    async def test_health_endpoint(self, async_client):
        """Test health endpoint"""
        start_time = time.time()
        
        response = await async_client.get("/health")
        
        end_time = time.time()
        response_time = (end_time - start_time) * 1000
        
        assert response.status_code == 200
        assert response_time < 100
        
        print(f"Health endpoint response time: {response_time:.2f}ms")
