"""
End-to-End tests for Robot Control System
Tests complete robot control workflow from API to hardware simulation
"""

import pytest
import asyncio
import time
from datetime import datetime, timezone
from unittest.mock import AsyncMock, patch

from app.services.robot_control_system import (
    robot_control_system, RobotState, MovementMode, Position
)
from app.services.speed_control import SpeedMode, speed_controller
from app.services.safety_speed_control import safety_speed_control


class TestRobotControlE2E:
    """End-to-End tests for robot control system"""
    
    @pytest.fixture
    def system(self):
        """Get robot control system instance"""
        return robot_control_system
    
    @pytest.fixture
    def test_positions(self):
        """Test positions for movement testing"""
        return [
            Position(1.0, 1.0, 0.0),    # Move to (1,1)
            Position(2.0, 2.0, 1.57),   # Move to (2,2) with rotation
            Position(0.0, 0.0, 0.0),    # Return to origin
        ]
    
    @pytest.mark.asyncio
    async def test_complete_movement_workflow(self, system, test_positions):
        """Test complete movement workflow from start to finish"""
        # Reset system
        await system.reset_system()
        
        # Test sequence of movements
        for i, target_pos in enumerate(test_positions):
            result = await system.move_to_position(
                target_position=target_pos,
                target_speed=1.0,
                movement_mode=MovementMode.AUTO
            )
            
            assert result["success"] is True, f"Movement {i+1} failed: {result.get('error')}"
            assert result["current_position"]["x"] == target_pos.x
            assert result["current_position"]["y"] == target_pos.y
            assert result["state"] == "idle"  # Should be idle after reaching target
            assert result["response_time_ms"] < 100.0  # E2E target: < 100ms
        
        # Verify final position
        status = await system.get_system_status()
        assert status["success"] is True
        assert status["current_position"]["x"] == 0.0
        assert status["current_position"]["y"] == 0.0
        assert status["robot_state"] == "idle"
    
    @pytest.mark.asyncio
    async def test_emergency_stop_workflow(self, system):
        """Test emergency stop workflow"""
        # Start movement
        target_pos = Position(5.0, 5.0, 0.0)
        result = await system.move_to_position(
            target_position=target_pos,
            target_speed=2.0,
            movement_mode=MovementMode.AUTO
        )
        
        assert result["success"] is True
        
        # Check if robot is still moving or has reached target
        status = await system.get_system_status()
        robot_state = status["robot_state"]
        
        # Trigger emergency stop (regardless of current state)
        emergency_result = await system.emergency_stop()
        
        assert emergency_result["success"] is True
        assert emergency_result["state"] == "emergency_stop"
        assert emergency_result["response_time_ms"] < 50.0  # Emergency stop target: < 50ms
        assert emergency_result["speed_control_success"] is True
        assert emergency_result["safety_system_success"] is True
        
        # Verify system is in emergency state
        status = await system.get_system_status()
        assert status["robot_state"] == "emergency_stop"
    
    @pytest.mark.asyncio
    async def test_movement_mode_switching(self, system):
        """Test switching between movement modes"""
        # Test manual mode
        result = await system.set_movement_mode(MovementMode.MANUAL)
        assert result["success"] is True
        assert result["movement_mode"] == "manual"
        assert result["speed_mode"] == "manual"
        
        # Test auto mode
        result = await system.set_movement_mode(MovementMode.AUTO)
        assert result["success"] is True
        assert result["movement_mode"] == "auto"
        assert result["speed_mode"] == "auto"
        
        # Test semi-auto mode
        result = await system.set_movement_mode(MovementMode.SEMI_AUTO)
        assert result["success"] is True
        assert result["movement_mode"] == "semi_auto"
        assert result["speed_mode"] == "auto"
        
        # Test maintenance mode
        result = await system.set_movement_mode(MovementMode.MAINTENANCE)
        assert result["success"] is True
        assert result["movement_mode"] == "maintenance"
        assert result["speed_mode"] == "safety"
    
    @pytest.mark.asyncio
    async def test_safety_integration_workflow(self, system):
        """Test safety system integration during movement"""
        # Reset system to clear any previous state
        await system.reset_system()
        
        # Simulate obstacle detection
        obstacle_data = [
            {
                "distance": 2.0,
                "angle": 0.0,
                "type": "static",
                "confidence": 0.8
            }
        ]
        
        # Update obstacles
        await safety_speed_control.update_obstacles(obstacle_data)
        
        # Try to move (should be limited by safety)
        target_pos = Position(3.0, 0.0, 0.0)  # Moving toward obstacle
        result = await system.move_to_position(
            target_position=target_pos,
            target_speed=2.0,
            movement_mode=MovementMode.AUTO
        )
        
        # Movement should succeed but speed should be limited
        assert result["success"] is True
        
        # Check safety status
        status = await system.get_system_status()
        assert status["success"] is True
        assert "safety_status" in status
        assert status["safety_status"]["obstacles_detected"] == 1
    
    @pytest.mark.asyncio
    async def test_concurrent_operations(self, system):
        """Test concurrent operations on the system"""
        # Reset system
        await system.reset_system()
        
        # Define concurrent operations
        async def move_operation():
            return await system.move_to_position(
                target_position=Position(1.0, 1.0, 0.0),
                target_speed=1.0,
                movement_mode=MovementMode.AUTO
            )
        
        async def mode_switch_operation():
            return await system.set_movement_mode(MovementMode.MANUAL)
        
        async def status_check_operation():
            return await system.get_system_status()
        
        # Execute operations concurrently
        tasks = [
            move_operation(),
            mode_switch_operation(),
            status_check_operation(),
            move_operation(),  # Another movement
            status_check_operation()
        ]
        
        results = await asyncio.gather(*tasks, return_exceptions=True)
        
        # Check that all operations completed without exceptions
        for i, result in enumerate(results):
            if isinstance(result, Exception):
                pytest.fail(f"Operation {i} failed with exception: {result}")
        
        # Verify system is still functional
        final_status = await system.get_system_status()
        assert final_status["success"] is True
        assert final_status["robot_state"] in ["idle", "moving"]
    
    @pytest.mark.asyncio
    async def test_error_recovery_workflow(self, system):
        """Test error recovery and system resilience"""
        # Reset system
        await system.reset_system()
        
        # Test invalid movement (should fail gracefully)
        invalid_result = await system.move_to_position(
            target_position=Position(-100.0, -100.0, 0.0),  # Invalid position
            target_speed=100.0,  # Invalid speed
            movement_mode=MovementMode.AUTO
        )
        
        # Should fail but not crash system
        assert invalid_result["success"] is False
        assert "error" in invalid_result
        
        # System should still be functional
        status = await system.get_system_status()
        assert status["success"] is True
        assert status["performance"]["error_count"] > 0  # Should have recorded the error
        
        # Test recovery with valid movement
        recovery_result = await system.move_to_position(
            target_position=Position(1.0, 1.0, 0.0),
            target_speed=1.0,
            movement_mode=MovementMode.AUTO
        )
        
        assert recovery_result["success"] is True
        assert recovery_result["state"] == "idle"
    
    @pytest.mark.asyncio
    async def test_performance_under_load(self, system):
        """Test system performance under load"""
        # Reset system
        await system.reset_system()
        
        num_operations = 50
        response_times = []
        
        # Execute many operations
        for i in range(num_operations):
            start_time = time.perf_counter()
            
            result = await system.move_to_position(
                target_position=Position(i % 5, i % 5, 0.0),
                target_speed=1.0,
                movement_mode=MovementMode.AUTO
            )
            
            end_time = time.perf_counter()
            response_time_ms = (end_time - start_time) * 1000
            response_times.append(response_time_ms)
            
            assert result["success"] is True
        
        # Check performance metrics
        avg_response_time = sum(response_times) / len(response_times)
        max_response_time = max(response_times)
        
        print(f"\nE2E Performance Results:")
        print(f"  Operations: {num_operations}")
        print(f"  Average response time: {avg_response_time:.2f} ms")
        print(f"  Max response time: {max_response_time:.2f} ms")
        
        # Performance assertions
        assert avg_response_time < 100.0, f"Average E2E response time {avg_response_time:.2f}ms exceeds 100ms target"
        assert max_response_time < 200.0, f"Max E2E response time {max_response_time:.2f}ms exceeds 200ms target"
        
        # Check system status
        status = await system.get_system_status()
        assert status["success"] is True
        assert status["performance"]["operation_count"] == num_operations
        assert status["performance"]["error_rate"] < 10.0  # Error rate should be low
    
    @pytest.mark.asyncio
    async def test_system_reset_workflow(self, system):
        """Test system reset functionality"""
        # Perform some operations to change state
        await system.move_to_position(
            target_position=Position(2.0, 2.0, 0.0),
            target_speed=1.5,
            movement_mode=MovementMode.AUTO
        )
        
        await system.set_movement_mode(MovementMode.MANUAL)
        
        # Get status before reset
        status_before = await system.get_system_status()
        assert status_before["success"] is True
        assert status_before["performance"]["operation_count"] > 0
        
        # Reset system
        reset_result = await system.reset_system()
        assert reset_result["success"] is True
        assert "reset successfully" in reset_result["message"]
        
        # Get status after reset
        status_after = await system.get_system_status()
        assert status_after["success"] is True
        assert status_after["robot_state"] == "idle"
        assert status_after["movement_mode"] == "manual"
        assert status_after["current_position"]["x"] == 0.0
        assert status_after["current_position"]["y"] == 0.0
        assert status_after["performance"]["operation_count"] == 0
        assert status_after["performance"]["error_count"] == 0
    
    @pytest.mark.asyncio
    async def test_complete_mission_workflow(self, system):
        """Test complete mission workflow"""
        # Reset system
        await system.reset_system()
        
        # Mission: Navigate through waypoints
        waypoints = [
            Position(1.0, 0.0, 0.0),    # Move forward
            Position(1.0, 1.0, 1.57),   # Turn right and move
            Position(0.0, 1.0, 3.14),   # Turn around and move back
            Position(0.0, 0.0, 0.0),    # Return to start
        ]
        
        mission_results = []
        
        for i, waypoint in enumerate(waypoints):
            result = await system.move_to_position(
                target_position=waypoint,
                target_speed=1.0,
                movement_mode=MovementMode.AUTO
            )
            
            mission_results.append(result)
            
            assert result["success"] is True, f"Waypoint {i+1} failed: {result.get('error')}"
            assert result["response_time_ms"] < 100.0
            
            # Small delay between waypoints
            await asyncio.sleep(0.1)
        
        # Verify mission completion
        final_status = await system.get_system_status()
        assert final_status["success"] is True
        assert final_status["robot_state"] == "idle"
        assert final_status["current_position"]["x"] == 0.0
        assert final_status["current_position"]["y"] == 0.0
        assert final_status["performance"]["operation_count"] == len(waypoints)
        
        print(f"\nMission Results:")
        for i, result in enumerate(mission_results):
            print(f"  Waypoint {i+1}: {result['response_time_ms']:.2f}ms")
        
        # All waypoints should have been reached successfully
        assert len(mission_results) == len(waypoints)
        assert all(result["success"] for result in mission_results)


class TestRobotControlSystemIntegration:
    """Test integration between robot control system components"""
    
    @pytest.fixture
    def system(self):
        """Get robot control system instance"""
        return robot_control_system
    
    @pytest.mark.asyncio
    async def test_speed_control_integration(self, system):
        """Test speed control system integration"""
        # Reset system to clear any previous state
        await system.reset_system()
        
        # Clear any obstacle data from previous tests
        await safety_speed_control.update_obstacles([])
        
        # Reset speed controller state
        speed_controller.safety_active = False
        speed_controller.current_speed = 0.0
        
        # Test speed control through robot system
        result = await system.move_to_position(
            target_position=Position(1.0, 0.0, 0.0),
            target_speed=1.5,
            movement_mode=MovementMode.AUTO
        )
        
        assert result["success"] is True
        
        # Check speed control status
        status = await system.get_system_status()
        assert "speed_control" in status
        assert status["speed_control"]["current_speed"] == 1.5
        assert status["speed_control"]["mode"] == "auto"
    
    @pytest.mark.asyncio
    async def test_safety_system_integration(self, system):
        """Test safety system integration"""
        # Simulate safety event
        await safety_speed_control.update_obstacles([
            {
                "distance": 1.5,
                "angle": 0.0,
                "type": "person",
                "confidence": 0.9
            }
        ])
        
        # Check safety integration
        status = await system.get_system_status()
        assert "safety_status" in status
        assert status["safety_status"]["obstacles_detected"] == 1
        assert status["safety_status"]["closest_obstacle"]["type"] == "person"
    
    @pytest.mark.asyncio
    async def test_emergency_system_integration(self, system):
        """Test emergency system integration"""
        # Start movement
        await system.move_to_position(
            target_position=Position(2.0, 2.0, 0.0),
            target_speed=2.0,
            movement_mode=MovementMode.AUTO
        )
        
        # Trigger emergency stop
        emergency_result = await system.emergency_stop()
        
        assert emergency_result["success"] is True
        assert emergency_result["speed_control_success"] is True
        assert emergency_result["safety_system_success"] is True
        
        # Verify all systems are in emergency state
        status = await system.get_system_status()
        assert status["robot_state"] == "emergency_stop"
        assert status["speed_control"]["mode"] == "emergency"
        assert status["safety_status"]["emergency_stop_active"] is True


if __name__ == "__main__":
    pytest.main([__file__, "-v", "-s"])  # -s to show print statements
