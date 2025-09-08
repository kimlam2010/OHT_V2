"""
Tests for Robot Control API endpoints
"""

import pytest
from fastapi.testclient import TestClient
from unittest.mock import AsyncMock, patch, MagicMock
import os

# Set testing environment
os.environ["TESTING"] = "true"

from app.main import app
from app.models.user import User
from app.core.security import get_current_user

# Mock user for testing
mock_user = User(
    id=1,
    username="test_user",
    email="test@example.com",
    role="admin",
    is_active=True
)

# Override the dependency for testing
app.dependency_overrides[get_current_user] = lambda: mock_user

client = TestClient(app)


class TestRobotControlAPI:
    """Test cases for Robot Control API endpoints"""
    
    def test_get_robot_status(self):
        """Test robot status endpoint"""
        response = client.get("/api/v1/robot/status")
        assert response.status_code == 200
        
        data = response.json()
        assert "robot_id" in data
        assert "status" in data
        assert "mode" in data
        assert "position" in data
        assert "battery_level" in data
        assert "temperature" in data
        assert "speed" in data
        assert "uptime" in data
        assert "health_score" in data
        assert "timestamp" in data
    
    def test_set_robot_mode(self):
        """Test set robot mode endpoint"""
        mode_data = {
            "mode": "manual",
            "reason": "Manual control required",
            "force": False
        }
        
        response = client.post("/api/v1/robot/mode", json=mode_data)
        assert response.status_code == 200
        
        data = response.json()
        assert data["success"] is True
        assert "message" in data
        assert "previous_mode" in data
        assert "current_mode" in data
        assert data["current_mode"] == "manual"
        assert "timestamp" in data
    
    def test_get_robot_mode(self):
        """Test get robot mode endpoint"""
        response = client.get("/api/v1/robot/mode")
        assert response.status_code == 200
        
        data = response.json()
        assert data["success"] is True
        assert "mode" in data
        assert "timestamp" in data
    
    def test_move_forward(self):
        """Test move forward endpoint"""
        response = client.post("/api/v1/robot/move/forward?speed=0.5")
        assert response.status_code == 200
        
        data = response.json()
        assert data["success"] is True
        assert data["direction"] == "forward"
        assert data["speed"] == 0.5
        assert "estimated_duration" in data
        assert "timestamp" in data
    
    def test_move_backward(self):
        """Test move backward endpoint"""
        response = client.post("/api/v1/robot/move/backward?speed=0.3")
        assert response.status_code == 200
        
        data = response.json()
        assert data["success"] is True
        assert data["direction"] == "backward"
        assert data["speed"] == 0.3
        assert "estimated_duration" in data
        assert "timestamp" in data
    
    def test_turn_left(self):
        """Test turn left endpoint"""
        response = client.post("/api/v1/robot/move/left?angle=45.0")
        assert response.status_code == 200
        
        data = response.json()
        assert data["success"] is True
        assert data["direction"] == "left"
        assert "estimated_duration" in data
        assert "timestamp" in data
    
    def test_turn_right(self):
        """Test turn right endpoint"""
        response = client.post("/api/v1/robot/move/right?angle=90.0")
        assert response.status_code == 200
        
        data = response.json()
        assert data["success"] is True
        assert data["direction"] == "right"
        assert "estimated_duration" in data
        assert "timestamp" in data
    
    def test_stop_movement(self):
        """Test stop movement endpoint"""
        response = client.post("/api/v1/robot/move/stop")
        assert response.status_code == 200
        
        data = response.json()
        assert data["success"] is True
        assert data["direction"] == "stop"
        assert data["speed"] == 0.0
        assert data["estimated_duration"] == 0.0
        assert "timestamp" in data
    
    def test_set_robot_speed(self):
        """Test set robot speed endpoint"""
        speed_data = {
            "speed": 0.7,
            "acceleration": 0.3,
            "deceleration": 0.4
        }
        
        response = client.post("/api/v1/robot/speed", json=speed_data)
        assert response.status_code == 200
        
        data = response.json()
        assert data["success"] is True
        assert data["current_speed"] == 0.7
        assert data["acceleration"] == 0.3
        assert data["deceleration"] == 0.4
        assert "timestamp" in data
    
    def test_get_robot_speed(self):
        """Test get robot speed endpoint"""
        response = client.get("/api/v1/robot/speed")
        assert response.status_code == 200
        
        data = response.json()
        assert data["success"] is True
        assert "speed" in data
        assert "timestamp" in data
    
    def test_set_speed_preset(self):
        """Test set speed preset endpoint"""
        preset_data = {
            "preset": "fast"
        }
        
        response = client.post("/api/v1/robot/speed/preset", json=preset_data)
        assert response.status_code == 200
        
        data = response.json()
        assert data["success"] is True
        assert data["preset"] == "fast"
        assert data["speed_value"] == 0.75  # fast preset speed
        assert "timestamp" in data
    
    def test_pause_system(self):
        """Test pause system endpoint"""
        response = client.post("/api/v1/robot/pause")
        assert response.status_code == 200
        
        data = response.json()
        assert data["success"] is True
        assert data["status"] == "paused"
        assert "timestamp" in data
    
    def test_reset_system(self):
        """Test reset system endpoint"""
        response = client.post("/api/v1/robot/reset")
        assert response.status_code == 200
        
        data = response.json()
        assert data["success"] is True
        assert data["status"] == "idle"
        assert "timestamp" in data
    
    def test_emergency_stop(self):
        """Test emergency stop endpoint"""
        response = client.post("/api/v1/robot/emergency-stop")
        assert response.status_code == 200
        
        data = response.json()
        assert data["success"] is True
        assert "message" in data
        assert "status" in data
        assert "timestamp" in data
    
    def test_get_robot_position(self):
        """Test get robot position endpoint"""
        response = client.get("/api/v1/robot/position")
        assert response.status_code == 200
        
        data = response.json()
        assert data["success"] is True
        assert "position" in data
        assert "last_updated" in data
        
        position = data["position"]
        assert "x" in position
        assert "y" in position
        assert "z" in position
        assert "orientation" in position
        assert "accuracy" in position
        assert "timestamp" in position
    
    def test_get_robot_configuration(self):
        """Test get robot configuration endpoint"""
        response = client.get("/api/v1/robot/configuration")
        assert response.status_code == 200
        
        data = response.json()
        assert data["success"] is True
        assert "configuration" in data
        assert "last_updated" in data
        
        config = data["configuration"]
        assert "max_speed" in config
        assert "max_acceleration" in config
        assert "max_deceleration" in config
        assert "safety_distance" in config
        assert "emergency_stop_timeout" in config
        assert "battery_warning_threshold" in config
        assert "temperature_warning_threshold" in config
        assert "auto_dock_enabled" in config
        assert "obstacle_detection_enabled" in config
        assert "logging_level" in config
    
    def test_emergency_stop_performance(self):
        """Test emergency stop response time"""
        import time
        
        start_time = time.time()
        response = client.post("/api/v1/robot/emergency-stop")
        end_time = time.time()
        
        response_time = (end_time - start_time) * 1000  # Convert to milliseconds
        assert response.status_code == 200
        assert response_time < 10  # Emergency stop should be under 10ms
    
    def test_robot_status_performance(self):
        """Test robot status response time"""
        import time
        
        start_time = time.time()
        response = client.get("/api/v1/robot/status")
        end_time = time.time()
        
        response_time = (end_time - start_time) * 1000  # Convert to milliseconds
        assert response.status_code == 200
        assert response_time < 50  # Should be under 50ms
    
    def test_move_commands_performance(self):
        """Test move commands response time"""
        import time
        
        commands = [
            "/api/v1/robot/move/forward?speed=0.5",
            "/api/v1/robot/move/backward?speed=0.3",
            "/api/v1/robot/move/left?angle=45.0",
            "/api/v1/robot/move/right?angle=90.0",
            "/api/v1/robot/move/stop"
        ]
        
        for command in commands:
            start_time = time.time()
            response = client.post(command)
            end_time = time.time()
            
            response_time = (end_time - start_time) * 1000  # Convert to milliseconds
            assert response.status_code == 200
            assert response_time < 50  # Should be under 50ms
    
    def test_speed_validation(self):
        """Test speed parameter validation"""
        # Test invalid speed (too high)
        response = client.post("/api/v1/robot/move/forward?speed=1.5")
        assert response.status_code == 422  # Validation error
        
        # Test invalid speed (negative)
        response = client.post("/api/v1/robot/move/forward?speed=-0.1")
        assert response.status_code == 422  # Validation error
        
        # Test valid speed
        response = client.post("/api/v1/robot/move/forward?speed=0.5")
        assert response.status_code == 200
    
    def test_angle_validation(self):
        """Test angle parameter validation"""
        # Test invalid angle (too high)
        response = client.post("/api/v1/robot/move/left?angle=400.0")
        assert response.status_code == 422  # Validation error
        
        # Test invalid angle (negative)
        response = client.post("/api/v1/robot/move/left?angle=-10.0")
        assert response.status_code == 422  # Validation error
        
        # Test valid angle
        response = client.post("/api/v1/robot/move/left?angle=45.0")
        assert response.status_code == 200
    
    def test_speed_request_validation(self):
        """Test speed request validation"""
        # Test invalid speed in request body
        speed_data = {
            "speed": 1.5,  # Too high
            "acceleration": 0.3,
            "deceleration": 0.4
        }
        
        response = client.post("/api/v1/robot/speed", json=speed_data)
        assert response.status_code == 422  # Validation error
        
        # Test valid speed in request body
        speed_data = {
            "speed": 0.7,
            "acceleration": 0.3,
            "deceleration": 0.4
        }
        
        response = client.post("/api/v1/robot/speed", json=speed_data)
        assert response.status_code == 200
