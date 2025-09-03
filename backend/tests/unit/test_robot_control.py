"""
Unit tests for robot control service
"""

import pytest
import os
from unittest.mock import AsyncMock, patch
from app.services.robot_control import robot_control_service


class TestRobotControlService:
    """Test robot control service"""
    
    @pytest.mark.asyncio
    async def test_get_robot_status(self):
        """Test get robot status"""
        # Set testing environment
        os.environ["TESTING"] = "true"
        
        result = await robot_control_service.get_robot_status()
        
        assert result["status"] == "idle"
        assert result["position"]["x"] == 150.5
        assert result["battery_level"] == 87
        assert result["temperature"] == 42.5
    
    @pytest.mark.asyncio
    async def test_send_command_success(self):
        """Test send command successfully"""
        command = {
            "command_type": "move",
            "parameters": {"direction": "forward", "speed": 1.0}
        }
        
        # Set testing environment
        os.environ["TESTING"] = "true"
        
        result = await robot_control_service.send_command(command)
        
        assert result["success"] is True
        assert result["command"]["command_type"] == command["command_type"]
        assert result["command"]["parameters"] == command["parameters"]
    
    @pytest.mark.asyncio
    async def test_send_command_failure(self):
        """Test send command failure"""
        command = {
            "command_type": "move",
            "parameters": {"direction": "forward", "speed": 1.0}
        }
        
        with patch.object(robot_control_service, '_firmware_service') as mock_firmware:
            mock_firmware.send_robot_command.return_value = False
            
            result = await robot_control_service.send_command(command)
            
            assert result["success"] is False
            assert "message" in result  # Changed from "error" to "message"
    
    @pytest.mark.asyncio
    async def test_emergency_stop(self):
        """Test emergency stop"""
        # Set testing environment
        os.environ["TESTING"] = "true"
        
        result = await robot_control_service.emergency_stop()
        
        assert result["success"] is True
        # Remove status check as it's not in the response format
    
    @pytest.mark.asyncio
    async def test_invalid_command_type(self):
        """Test invalid command type"""
        command = {
            "command_type": "invalid_command",
            "parameters": {}
        }
        
        # Set testing environment
        os.environ["TESTING"] = "true"
        
        result = await robot_control_service.send_command(command)
        
        assert result["success"] is False
        assert "message" in result  # Changed from "error" to "message"
        assert "Invalid command" in result["message"]
    
    @pytest.mark.asyncio
    async def test_command_validation(self):
        """Test command validation"""
        # Set testing environment
        os.environ["TESTING"] = "true"
        
        # Test missing command_type
        command = {"parameters": {}}
        result = await robot_control_service.send_command(command)
        assert result["success"] is False
        
        # Test missing parameters - this should pass because parameters are optional
        command = {"command_type": "move"}
        result = await robot_control_service.send_command(command)
        assert result["success"] is True  # Should pass with mock firmware
