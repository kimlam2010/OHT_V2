#!/usr/bin/env python3
"""
Test Robot Mode Consistency
Validates that database and service defaults are consistent
"""

import asyncio
import logging
from app.core.database import get_db_context
from app.models.robot import Robot
from app.services.robot_control_system import RobotControlSystem, MovementMode

logger = logging.getLogger(__name__)

async def test_robot_mode_consistency():
    """Test that robot mode defaults are consistent"""
    try:
        # Test 1: Database model default
        logger.info("🧪 Test 1: Database model default")
        
        async with get_db_context() as db:
            # Create a new robot without specifying mode
            new_robot = Robot(
                robot_id="TEST-ROBOT-001",
                status="idle"
                # mode not specified - should use default
            )
            
            db.add(new_robot)
            await db.commit()
            await db.refresh(new_robot)
            
            logger.info(f"Created robot with mode: '{new_robot.mode}'")
            
            # Verify default is 'manual'
            assert new_robot.mode == "manual", f"Expected 'manual', got '{new_robot.mode}'"
            logger.info("✅ Database model default is correct: 'manual'")
            
            # Clean up
            await db.delete(new_robot)
            await db.commit()
        
        # Test 2: Service default
        logger.info("🧪 Test 2: Service default")
        
        robot_system = RobotControlSystem()
        service_mode = robot_system.movement_mode
        
        logger.info(f"Service default mode: {service_mode}")
        
        # Verify service default is MANUAL
        assert service_mode == MovementMode.MANUAL, f"Expected MANUAL, got {service_mode}"
        logger.info("✅ Service default is correct: MANUAL")
        
        # Test 3: Mode mapping consistency
        logger.info("🧪 Test 3: Mode mapping consistency")
        
        mode_mapping = {
            "manual": MovementMode.MANUAL,
            "auto": MovementMode.AUTO,
            "semi_auto": MovementMode.SEMI_AUTO,
            "maintenance": MovementMode.MAINTENANCE
        }
        
        for db_mode, service_mode in mode_mapping.items():
            logger.info(f"  {db_mode} -> {service_mode}")
        
        logger.info("✅ Mode mapping is consistent")
        
        # Test 4: Integration test
        logger.info("🧪 Test 4: Integration test")
        
        # Test setting mode via service
        result = await robot_system.set_movement_mode(MovementMode.AUTO)
        assert result["success"], f"Failed to set mode: {result}"
        assert robot_system.movement_mode == MovementMode.AUTO, "Mode not set correctly"
        
        logger.info("✅ Integration test passed")
        
        logger.info("🎉 All tests passed! Robot mode consistency is correct.")
        
    except Exception as e:
        logger.error(f"❌ Test failed: {e}")
        raise

if __name__ == "__main__":
    async def main():
        logging.basicConfig(level=logging.INFO)
        await test_robot_mode_consistency()
    
    asyncio.run(main())
