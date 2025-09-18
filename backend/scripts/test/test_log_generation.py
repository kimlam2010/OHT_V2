#!/usr/bin/env python3
"""
Test script to generate system logs for WebSocket testing
Tests Issue #78 - WebSocket cho system logs
"""

import asyncio
import logging
import sys
import os
from datetime import datetime, timezone

# Add project root to Python path
sys.path.insert(0, os.path.join(os.path.dirname(__file__), '..', '..'))

# Configure logging
logging.basicConfig(level=logging.DEBUG)
logger = logging.getLogger(__name__)


async def generate_test_logs():
    """Generate various types of system logs"""
    logger.info("📝 Generating test system logs...")
    
    # Different loggers for different sources
    robot_logger = logging.getLogger("robot_controller")
    safety_logger = logging.getLogger("safety_system")
    telemetry_logger = logging.getLogger("telemetry_service")
    network_logger = logging.getLogger("network_manager")
    system_logger = logging.getLogger("system_monitor")
    
    # Test scenarios with different log levels
    test_scenarios = [
        {
            "logger": robot_logger,
            "level": "info",
            "message": "Robot movement started - moving forward",
            "delay": 1
        },
        {
            "logger": telemetry_logger,
            "level": "debug", 
            "message": "Sensor data collected: temperature=45.2°C, battery=87%",
            "delay": 1
        },
        {
            "logger": safety_logger,
            "level": "warning",
            "message": "Safety zone proximity detected - reducing speed",
            "delay": 2
        },
        {
            "logger": network_logger,
            "level": "error",
            "message": "Failed to connect to center system - retrying...",
            "delay": 1
        },
        {
            "logger": system_logger,
            "level": "critical",
            "message": "System memory usage critical - 95% utilization",
            "delay": 2
        },
        {
            "logger": robot_logger,
            "level": "info",
            "message": "Robot reached target position (150.5, 200.3)",
            "delay": 1
        },
        {
            "logger": telemetry_logger,
            "level": "warning",
            "message": "Battery level low - 18% remaining",
            "delay": 1
        },
        {
            "logger": safety_logger,
            "level": "info",
            "message": "Emergency stop test completed successfully",
            "delay": 1
        },
        {
            "logger": network_logger,
            "level": "info",
            "message": "Connection to center system restored",
            "delay": 1
        },
        {
            "logger": system_logger,
            "level": "debug",
            "message": "Performance metrics collected: CPU=45%, Memory=67%",
            "delay": 1
        }
    ]
    
    logger.info(f"🚀 Generating {len(test_scenarios)} test log entries...")
    
    for i, scenario in enumerate(test_scenarios):
        try:
            # Get logger and log level
            test_logger = scenario["logger"]
            level = scenario["level"]
            message = scenario["message"]
            delay = scenario["delay"]
            
            # Log message at appropriate level
            if level == "debug":
                test_logger.debug(message)
            elif level == "info":
                test_logger.info(message)
            elif level == "warning":
                test_logger.warning(message)
            elif level == "error":
                test_logger.error(message)
            elif level == "critical":
                test_logger.critical(message)
            
            logger.info(f"📝 Generated log {i+1}: [{level.upper()}] {message[:50]}...")
            
            # Wait between logs
            await asyncio.sleep(delay)
            
        except Exception as e:
            logger.error(f"❌ Failed to generate log {i+1}: {e}")
    
    logger.info("🎯 Log generation completed!")


async def test_different_log_levels():
    """Test different log levels"""
    logger.info("🔧 Testing different log levels...")
    
    # Create loggers for different components
    components = [
        "robot_control", "safety_system", "telemetry_collector",
        "network_manager", "database_manager", "authentication",
        "monitoring_service", "alert_system", "configuration"
    ]
    
    levels = ["debug", "info", "warning", "error", "critical"]
    
    for component in components:
        comp_logger = logging.getLogger(component)
        
        for level in levels:
            message = f"Test {level} message from {component} - timestamp {datetime.now().isoformat()}"
            
            if level == "debug":
                comp_logger.debug(message)
            elif level == "info":
                comp_logger.info(message)
            elif level == "warning":
                comp_logger.warning(message)
            elif level == "error":
                comp_logger.error(message)
            elif level == "critical":
                comp_logger.critical(message)
            
            await asyncio.sleep(0.5)  # Quick succession
    
    logger.info("✅ Log level testing completed!")


async def simulate_system_activity():
    """Simulate real system activity with logs"""
    logger.info("🎭 Simulating system activity...")
    
    # Simulate robot operation cycle
    robot_logger = logging.getLogger("robot_controller")
    safety_logger = logging.getLogger("safety_monitor")
    telemetry_logger = logging.getLogger("telemetry_collector")
    
    # Robot startup sequence
    robot_logger.info("Robot system initializing...")
    await asyncio.sleep(1)
    
    safety_logger.info("Safety systems check: All systems nominal")
    await asyncio.sleep(1)
    
    telemetry_logger.debug("Sensor calibration started")
    await asyncio.sleep(1)
    
    robot_logger.info("Robot ready for operation")
    await asyncio.sleep(1)
    
    # Movement cycle
    robot_logger.info("Starting movement to position (100, 200)")
    telemetry_logger.debug("Position update: (50, 100)")
    await asyncio.sleep(2)
    
    telemetry_logger.debug("Position update: (75, 150)")
    await asyncio.sleep(1)
    
    safety_logger.warning("Obstacle detected - adjusting path")
    await asyncio.sleep(1)
    
    robot_logger.info("Path adjusted - continuing movement")
    telemetry_logger.debug("Position update: (95, 190)")
    await asyncio.sleep(2)
    
    robot_logger.info("Target position reached successfully")
    telemetry_logger.info("Movement cycle completed - total time: 8.5 seconds")
    
    logger.info("🎭 System activity simulation completed!")


async def main():
    """Main test function"""
    logger.info("🚀 Starting Log Generation Test for Issue #78")
    logger.info("This will generate system logs that should appear in WebSocket /ws/logs")
    
    try:
        # Generate basic test logs
        await generate_test_logs()
        
        await asyncio.sleep(3)
        
        # Test different log levels
        await test_different_log_levels()
        
        await asyncio.sleep(3)
        
        # Simulate system activity
        await simulate_system_activity()
        
        logger.info("\n🎉 Log generation test completed!")
        logger.info("💡 To see WebSocket log streaming, run test_websocket_logs.py in another terminal")
        
    except KeyboardInterrupt:
        logger.info("🛑 Test interrupted by user")
    except Exception as e:
        logger.error(f"❌ Test failed: {e}")


if __name__ == "__main__":
    asyncio.run(main())
