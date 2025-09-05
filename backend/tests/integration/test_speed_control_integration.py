"""
Integration tests for Speed Control System
Tests speed control API endpoints and system integration
"""

import pytest
import asyncio
from httpx import AsyncClient
from fastapi.testclient import TestClient

from app.main import app
from app.services.speed_control import speed_controller
from app.schemas.user import UserResponse, UserRole, UserStatus
from datetime import datetime


class TestSpeedControlAPI:
    """Test Speed Control API endpoints"""
    
    @pytest.fixture
    def client(self):
        """Create test client with authentication override"""
        from app.api.deps import get_current_user
        
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
    
    def test_set_speed_success(self, client, auth_headers):
        """Test successful speed setting via API"""
        response = client.post(
            "/api/v1/speed-control/set-speed",
            json={
                "target_speed": 1.5,
                "mode": "manual",
                "safety_override": False
            },
            headers=auth_headers
        )
        
        assert response.status_code == 200
        data = response.json()
        
        assert data["success"] is True
        assert data["current_speed"] == 1.5
        assert data["target_speed"] == 1.5
        assert data["mode"] == "manual"
        assert data["response_time_ms"] < 5.0
        assert "timestamp" in data
    
    def test_set_speed_with_safety_limits(self, client, auth_headers):
        """Test speed setting with safety limits applied"""
        response = client.post(
            "/api/v1/speed-control/set-speed",
            json={
                "target_speed": 5.0,  # Above max limit
                "mode": "manual",
                "safety_override": False
            },
            headers=auth_headers
        )
        
        assert response.status_code == 200
        data = response.json()
        
        assert data["success"] is True
        assert data["current_speed"] == 2.0  # Should be limited to max_speed
        assert data["target_speed"] == 2.0
    
    def test_set_speed_safety_mode(self, client, auth_headers):
        """Test speed setting in safety mode"""
        response = client.post(
            "/api/v1/speed-control/set-speed",
            json={
                "target_speed": 1.0,
                "mode": "safety",
                "safety_override": False
            },
            headers=auth_headers
        )
        
        assert response.status_code == 200
        data = response.json()
        
        assert data["success"] is True
        assert data["current_speed"] == 0.5  # Should be limited to safety_speed
        assert data["target_speed"] == 0.5
        assert data["mode"] == "safety"
    
    def test_set_speed_emergency_mode(self, client, auth_headers):
        """Test speed setting in emergency mode"""
        response = client.post(
            "/api/v1/speed-control/set-speed",
            json={
                "target_speed": 1.0,
                "mode": "emergency",
                "safety_override": False
            },
            headers=auth_headers
        )
        
        assert response.status_code == 200
        data = response.json()
        
        assert data["success"] is True
        assert data["current_speed"] == 0.1  # Should be limited to emergency_speed
        assert data["target_speed"] == 0.1
        assert data["mode"] == "emergency"
    
    def test_set_speed_invalid_mode(self, client, auth_headers):
        """Test speed setting with invalid mode"""
        response = client.post(
            "/api/v1/speed-control/set-speed",
            json={
                "target_speed": 1.0,
                "mode": "invalid_mode",
                "safety_override": False
            },
            headers=auth_headers
        )
        
        assert response.status_code == 422
        data = response.json()
        assert "detail" in data
    
    def test_set_speed_invalid_speed(self, client, auth_headers):
        """Test speed setting with invalid speed value"""
        response = client.post(
            "/api/v1/speed-control/set-speed",
            json={
                "target_speed": -1.0,  # Invalid negative speed
                "mode": "manual",
                "safety_override": False
            },
            headers=auth_headers
        )
        
        assert response.status_code == 422
        data = response.json()
        assert "detail" in data
    
    def test_emergency_stop(self, client, auth_headers):
        """Test emergency stop via API"""
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
        
        # Then execute emergency stop
        response = client.post(
            "/api/v1/speed-control/emergency-stop",
            headers=auth_headers
        )
        
        assert response.status_code == 200
        data = response.json()
        
        assert data["success"] is True
        assert data["current_speed"] == 0.0
        assert data["target_speed"] == 0.0
        assert data["mode"] == "emergency"
        assert data["response_time_ms"] < 5.0
    
    def test_get_speed_status(self, client, auth_headers):
        """Test getting speed status via API"""
        response = client.get(
            "/api/v1/speed-control/status",
            headers=auth_headers
        )
        
        assert response.status_code == 200
        data = response.json()
        
        assert data["success"] is True
        assert "data" in data
        assert "timestamp" in data
        
        status_data = data["data"]
        assert "current_speed" in status_data
        assert "target_speed" in status_data
        assert "mode" in status_data
        assert "safety_active" in status_data
        assert "obstacle_detected" in status_data
        assert "limits" in status_data
        assert "performance" in status_data
    
    def test_get_performance_metrics(self, client, auth_headers):
        """Test getting performance metrics via API"""
        # Execute some commands first
        client.post(
            "/api/v1/speed-control/set-speed",
            json={
                "target_speed": 1.0,
                "mode": "manual",
                "safety_override": False
            },
            headers=auth_headers
        )
        
        response = client.get(
            "/api/v1/speed-control/performance",
            headers=auth_headers
        )
        
        assert response.status_code == 200
        data = response.json()
        
        assert data["success"] is True
        assert "data" in data
        assert "timestamp" in data
        
        metrics = data["data"]
        assert "total_commands" in metrics
        assert "avg_response_time_ms" in metrics
        assert "min_response_time_ms" in metrics
        assert "max_response_time_ms" in metrics
        assert "response_time_target_ms" in metrics
        assert "target_met" in metrics
    
    def test_configure_speed_limits(self, client, auth_headers):
        """Test configuring speed limits via API"""
        response = client.post(
            "/api/v1/speed-control/configure-limits",
            json={
                "max_speed": 3.0,
                "max_acceleration": 1.5,
                "max_deceleration": 3.0,
                "safety_speed": 0.8,
                "emergency_speed": 0.2
            },
            headers=auth_headers
        )
        
        assert response.status_code == 200
        data = response.json()
        
        assert data["success"] is True
        assert data["message"] == "Speed limits updated successfully"
        assert "limits" in data
        assert "timestamp" in data
        
        limits = data["limits"]
        assert limits["max_speed"] == 3.0
        assert limits["max_acceleration"] == 1.5
        assert limits["max_deceleration"] == 3.0
        assert limits["safety_speed"] == 0.8
        assert limits["emergency_speed"] == 0.2
    
    def test_update_safety_status(self, client, auth_headers):
        """Test updating safety status via API"""
        response = client.post(
            "/api/v1/speed-control/safety-status",
            params={
                "safety_active": True,
                "obstacle_detected": True
            },
            headers=auth_headers
        )
        
        assert response.status_code == 200
        data = response.json()
        
        assert data["success"] is True
        assert data["message"] == "Safety status updated successfully"
        assert data["safety_active"] is True
        assert data["obstacle_detected"] is True
        assert "timestamp" in data
    
    def test_unauthorized_access(self, client):
        """Test unauthorized access to speed control endpoints"""
        response = client.post(
            "/api/v1/speed-control/set-speed",
            json={
                "target_speed": 1.0,
                "mode": "manual",
                "safety_override": False
            }
        )
        
        assert response.status_code == 401
    
    def test_insufficient_permissions(self, client):
        """Test insufficient permissions for speed control"""
        # Use viewer role (insufficient permissions)
        headers = {
            "Authorization": "Bearer test_token",
            "X-User-Role": "viewer",
            "X-User-Id": "test_viewer"
        }
        
        response = client.post(
            "/api/v1/speed-control/set-speed",
            json={
                "target_speed": 1.0,
                "mode": "manual",
                "safety_override": False
            },
            headers=headers
        )
        
        assert response.status_code == 403


class TestSpeedControlIntegration:
    """Test Speed Control system integration"""
    
    @pytest.fixture
    def client(self):
        """Create test client with authentication override"""
        from app.api.deps import get_current_user
        
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
    
    def test_speed_control_workflow(self, client, auth_headers):
        """Test complete speed control workflow"""
        # 1. Get initial status
        response = client.get(
            "/api/v1/speed-control/status",
            headers=auth_headers
        )
        assert response.status_code == 200
        initial_speed = response.json()["data"]["current_speed"]
        
        # 2. Set speed to manual mode
        response = client.post(
            "/api/v1/speed-control/set-speed",
            json={
                "target_speed": 1.5,
                "mode": "manual",
                "safety_override": False
            },
            headers=auth_headers
        )
        assert response.status_code == 200
        assert response.json()["current_speed"] == 1.5
        
        # 3. Switch to safety mode
        response = client.post(
            "/api/v1/speed-control/set-speed",
            json={
                "target_speed": 1.0,
                "mode": "safety",
                "safety_override": False
            },
            headers=auth_headers
        )
        assert response.status_code == 200
        assert response.json()["current_speed"] == 0.5  # Limited by safety speed
        
        # 4. Emergency stop
        response = client.post(
            "/api/v1/speed-control/emergency-stop",
            headers=auth_headers
        )
        assert response.status_code == 200
        assert response.json()["current_speed"] == 0.0
        
        # 5. Verify final status
        response = client.get(
            "/api/v1/speed-control/status",
            headers=auth_headers
        )
        assert response.status_code == 200
        assert response.json()["data"]["current_speed"] == 0.0
    
    def test_performance_under_load(self, client, auth_headers):
        """Test speed control performance under load"""
        import time
        
        # Execute multiple speed commands rapidly
        start_time = time.time()
        
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
            assert response.json()["response_time_ms"] < 5.0
        
        end_time = time.time()
        total_time = end_time - start_time
        
        # Should complete 10 commands in reasonable time
        assert total_time < 1.0  # Less than 1 second for 10 commands
        
        # Check performance metrics
        response = client.get(
            "/api/v1/speed-control/performance",
            headers=auth_headers
        )
        assert response.status_code == 200
        
        metrics = response.json()["data"]
        assert metrics["total_commands"] >= 10
        assert metrics["target_met"] is True  # All commands should meet performance target
    
    def test_safety_integration(self, client, auth_headers):
        """Test safety system integration"""
        # 1. Set normal speed
        response = client.post(
            "/api/v1/speed-control/set-speed",
            json={
                "target_speed": 1.5,
                "mode": "manual",
                "safety_override": False
            },
            headers=auth_headers
        )
        assert response.status_code == 200
        assert response.json()["current_speed"] == 1.5
        
        # 2. Activate safety status
        response = client.post(
            "/api/v1/speed-control/safety-status",
            params={
                "safety_active": True,
                "obstacle_detected": True
            },
            headers=auth_headers
        )
        assert response.status_code == 200
        
        # 3. Try to set speed (should be limited by safety)
        response = client.post(
            "/api/v1/speed-control/set-speed",
            json={
                "target_speed": 1.0,
                "mode": "manual",
                "safety_override": False
            },
            headers=auth_headers
        )
        assert response.status_code == 200
        # Speed should be limited by safety system
        assert response.json()["current_speed"] <= 0.5  # Safety speed limit
    
    def test_error_handling(self, client, auth_headers):
        """Test error handling in speed control"""
        # Test with invalid JSON
        response = client.post(
            "/api/v1/speed-control/set-speed",
            json={
                "target_speed": "invalid",  # Invalid type
                "mode": "manual",
                "safety_override": False
            },
            headers=auth_headers
        )
        assert response.status_code == 422  # Validation error
        
        # Test with missing required fields
        response = client.post(
            "/api/v1/speed-control/set-speed",
            json={
                "mode": "manual",
                "safety_override": False
                # Missing target_speed
            },
            headers=auth_headers
        )
        assert response.status_code == 422  # Validation error
