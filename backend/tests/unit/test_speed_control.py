"""
Unit tests for Speed Control Service
Tests speed controller functionality and performance
"""

import pytest
import asyncio
from datetime import datetime, timezone
from unittest.mock import AsyncMock, patch

from app.services.speed_control import (
    SpeedController, SpeedMode, SpeedLimits, SpeedCommand
)


class TestSpeedController:
    """Test SpeedController class"""
    
    @pytest.fixture
    def speed_controller(self):
        """Create speed controller instance for testing"""
        limits = SpeedLimits(
            max_speed=2.0,
            max_acceleration=1.0,
            max_deceleration=2.0,
            safety_speed=0.5,
            emergency_speed=0.1
        )
        return SpeedController(limits)
    
    @pytest.mark.asyncio
    async def test_set_speed_success(self, speed_controller):
        """Test successful speed setting"""
        result = await speed_controller.set_speed(1.5, SpeedMode.MANUAL)
        
        assert result["success"] is True
        assert result["current_speed"] == 1.5
        assert result["target_speed"] == 1.5
        assert result["mode"] == "manual"
        assert result["response_time_ms"] < 5.0  # Performance target
        assert "timestamp" in result
    
    @pytest.mark.asyncio
    async def test_set_speed_with_safety_limits(self, speed_controller):
        """Test speed setting with safety limits applied"""
        # Try to set speed above max limit
        result = await speed_controller.set_speed(5.0, SpeedMode.MANUAL)
        
        assert result["success"] is True
        assert result["current_speed"] == 2.0  # Should be limited to max_speed
        assert result["target_speed"] == 2.0
    
    @pytest.mark.asyncio
    async def test_set_speed_safety_mode(self, speed_controller):
        """Test speed setting in safety mode"""
        result = await speed_controller.set_speed(1.0, SpeedMode.SAFETY)
        
        assert result["success"] is True
        assert result["current_speed"] == 0.5  # Should be limited to safety_speed
        assert result["target_speed"] == 0.5
        assert result["mode"] == "safety"
    
    @pytest.mark.asyncio
    async def test_set_speed_emergency_mode(self, speed_controller):
        """Test speed setting in emergency mode"""
        result = await speed_controller.set_speed(1.0, SpeedMode.EMERGENCY)
        
        assert result["success"] is True
        assert result["current_speed"] == 0.1  # Should be limited to emergency_speed
        assert result["target_speed"] == 0.1
        assert result["mode"] == "emergency"
    
    @pytest.mark.asyncio
    async def test_set_speed_with_obstacle_detected(self, speed_controller):
        """Test speed setting when obstacle is detected"""
        # Simulate obstacle detection
        speed_controller.obstacle_detected = True
        
        result = await speed_controller.set_speed(1.0, SpeedMode.MANUAL)
        
        assert result["success"] is True
        assert result["current_speed"] == 0.5  # Should be limited to safety_speed
        assert result["target_speed"] == 0.5
    
    @pytest.mark.asyncio
    async def test_set_speed_with_safety_override(self, speed_controller):
        """Test speed setting with safety override"""
        result = await speed_controller.set_speed(1.5, SpeedMode.MANUAL, safety_override=True)
        
        assert result["success"] is True
        assert result["current_speed"] == 1.5  # Should respect override
        assert result["target_speed"] == 1.5
    
    @pytest.mark.asyncio
    async def test_set_speed_invalid_input(self, speed_controller):
        """Test speed setting with invalid input"""
        result = await speed_controller.set_speed(-1.0, SpeedMode.MANUAL)
        
        assert result["success"] is False
        assert "error" in result
        assert result["error"] == "Invalid speed input"
    
    @pytest.mark.asyncio
    async def test_emergency_stop(self, speed_controller):
        """Test emergency stop functionality"""
        # Set initial speed
        await speed_controller.set_speed(1.5, SpeedMode.MANUAL)
        
        # Execute emergency stop
        result = await speed_controller.emergency_stop()
        
        assert result["success"] is True
        assert result["current_speed"] == 0.0
        assert result["target_speed"] == 0.0
        assert result["mode"] == "emergency"
        assert result["response_time_ms"] < 5.0  # Performance target
    
    def test_set_safety_status(self, speed_controller):
        """Test safety status setting"""
        speed_controller.set_safety_status(True, True)
        
        assert speed_controller.safety_active is True
        assert speed_controller.obstacle_detected is True
    
    def test_get_status(self, speed_controller):
        """Test getting speed controller status"""
        status = speed_controller.get_status()
        
        assert "current_speed" in status
        assert "target_speed" in status
        assert "mode" in status
        assert "safety_active" in status
        assert "obstacle_detected" in status
        assert "limits" in status
        assert "performance" in status
        
        # Check limits structure
        limits = status["limits"]
        assert "max_speed" in limits
        assert "max_acceleration" in limits
        assert "max_deceleration" in limits
        assert "safety_speed" in limits
        assert "emergency_speed" in limits
    
    def test_get_performance_metrics_empty(self, speed_controller):
        """Test getting performance metrics when no commands executed"""
        metrics = speed_controller.get_performance_metrics()
        
        assert "error" in metrics
        assert metrics["error"] == "No performance data available"
    
    @pytest.mark.asyncio
    async def test_get_performance_metrics_with_data(self, speed_controller):
        """Test getting performance metrics with command data"""
        # Execute some commands
        await speed_controller.set_speed(1.0, SpeedMode.MANUAL)
        await speed_controller.set_speed(1.5, SpeedMode.MANUAL)
        
        metrics = speed_controller.get_performance_metrics()
        
        assert "total_commands" in metrics
        assert "avg_response_time_ms" in metrics
        assert "min_response_time_ms" in metrics
        assert "max_response_time_ms" in metrics
        assert "response_time_target_ms" in metrics
        assert "target_met" in metrics
        
        assert metrics["total_commands"] == 2
        assert metrics["response_time_target_ms"] == 5.0
        assert isinstance(metrics["target_met"], bool)
    
    @pytest.mark.asyncio
    async def test_performance_target_met(self, speed_controller):
        """Test that performance target is met"""
        result = await speed_controller.set_speed(1.0, SpeedMode.MANUAL)
        
        assert result["response_time_ms"] < 5.0
        assert result["success"] is True
    
    @pytest.mark.asyncio
    async def test_acceleration_calculation(self, speed_controller):
        """Test acceleration calculation"""
        # Start from 0 speed
        speed_controller.current_speed = 0.0
        
        # Set target speed
        result = await speed_controller.set_speed(1.0, SpeedMode.MANUAL)
        
        assert result["success"] is True
        assert speed_controller.current_speed == 1.0
    
    @pytest.mark.asyncio
    async def test_deceleration_calculation(self, speed_controller):
        """Test deceleration calculation"""
        # Start from high speed
        speed_controller.current_speed = 2.0
        
        # Set lower target speed
        result = await speed_controller.set_speed(1.0, SpeedMode.MANUAL)
        
        assert result["success"] is True
        assert speed_controller.current_speed == 1.0
    
    def test_speed_limits_validation(self, speed_controller):
        """Test speed limits validation"""
        limits = speed_controller.limits
        
        assert limits.max_speed == 2.0
        assert limits.max_acceleration == 1.0
        assert limits.max_deceleration == 2.0
        assert limits.safety_speed == 0.5
        assert limits.emergency_speed == 0.1
    
    @pytest.mark.asyncio
    async def test_concurrent_speed_commands(self, speed_controller):
        """Test concurrent speed commands"""
        # Execute multiple commands concurrently
        tasks = [
            speed_controller.set_speed(1.0, SpeedMode.MANUAL),
            speed_controller.set_speed(1.5, SpeedMode.MANUAL),
            speed_controller.set_speed(0.5, SpeedMode.MANUAL)
        ]
        
        results = await asyncio.gather(*tasks)
        
        # All commands should succeed
        for result in results:
            assert result["success"] is True
        
        # Final speed should be the last command
        assert speed_controller.current_speed == 0.5


class TestSpeedLimits:
    """Test SpeedLimits dataclass"""
    
    def test_default_limits(self):
        """Test default speed limits"""
        limits = SpeedLimits()
        
        assert limits.max_speed == 2.0
        assert limits.max_acceleration == 1.0
        assert limits.max_deceleration == 2.0
        assert limits.safety_speed == 0.5
        assert limits.emergency_speed == 0.1
    
    def test_custom_limits(self):
        """Test custom speed limits"""
        limits = SpeedLimits(
            max_speed=3.0,
            max_acceleration=1.5,
            max_deceleration=3.0,
            safety_speed=0.8,
            emergency_speed=0.2
        )
        
        assert limits.max_speed == 3.0
        assert limits.max_acceleration == 1.5
        assert limits.max_deceleration == 3.0
        assert limits.safety_speed == 0.8
        assert limits.emergency_speed == 0.2


class TestSpeedCommand:
    """Test SpeedCommand dataclass"""
    
    def test_speed_command_creation(self):
        """Test speed command creation"""
        timestamp = datetime.now(timezone.utc)
        command = SpeedCommand(
            target_speed=1.5,
            acceleration=1.0,
            mode=SpeedMode.MANUAL,
            timestamp=timestamp,
            safety_override=False
        )
        
        assert command.target_speed == 1.5
        assert command.acceleration == 1.0
        assert command.mode == SpeedMode.MANUAL
        assert command.timestamp == timestamp
        assert command.safety_override is False
    
    def test_speed_command_defaults(self):
        """Test speed command with defaults"""
        timestamp = datetime.now(timezone.utc)
        command = SpeedCommand(
            target_speed=1.0,
            acceleration=0.5,
            mode=SpeedMode.AUTO,
            timestamp=timestamp
        )
        
        assert command.safety_override is False  # Default value


class TestSpeedMode:
    """Test SpeedMode enum"""
    
    def test_speed_mode_values(self):
        """Test speed mode enum values"""
        assert SpeedMode.MANUAL.value == "manual"
        assert SpeedMode.AUTO.value == "auto"
        assert SpeedMode.SAFETY.value == "safety"
        assert SpeedMode.EMERGENCY.value == "emergency"
    
    def test_speed_mode_from_string(self):
        """Test creating speed mode from string"""
        assert SpeedMode("manual") == SpeedMode.MANUAL
        assert SpeedMode("auto") == SpeedMode.AUTO
        assert SpeedMode("safety") == SpeedMode.SAFETY
        assert SpeedMode("emergency") == SpeedMode.EMERGENCY
